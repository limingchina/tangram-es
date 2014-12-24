#include "fontStyle.h"
#define GLFONTSTASH_IMPLEMENTATION
#include "fontstash/glfontstash.h"

FontStyle::FontStyle(const std::string& _fontFile, std::string _name, GLenum _drawMode) : Style(_name, _drawMode) {
    constructVertexLayout();
    constructShaderProgram();
    initFontContext(_fontFile);
}

FontStyle::~FontStyle() {
    glfonsDelete(m_fontContext);

    for(auto pair : m_tileTexTransforms) {
        glDeleteTextures(1, &pair.second);
    }
    
    glDeleteTextures(1, &m_atlas);
}

void FontStyle::constructVertexLayout() {
    m_vertexLayout = std::shared_ptr<VertexLayout>(new VertexLayout({
        {"a_position", 2, GL_FLOAT, false, 0},
        {"a_texCoord", 2, GL_FLOAT, false, 0},
        {"a_fsid", 1, GL_FLOAT, false, 0},
    }));
}

void FontStyle::constructShaderProgram() {
    std::string vertShaderSrcStr = stringFromResource("text.vs");
    std::string fragShaderSrcStr = stringFromResource("text.fs");

    m_shaderProgram = std::make_shared<ShaderProgram>();

    if(!m_shaderProgram->buildFromSourceStrings(fragShaderSrcStr, vertShaderSrcStr)) {
        logMsg("Error building text shader program\n");
    }
}

void FontStyle::buildPoint(Point& _point, std::string& _layer, Properties& _props, VboMesh& _mesh) {

}

void FontStyle::buildLine(Line& _line, std::string& _layer, Properties& _props, VboMesh& _mesh) {

    if(_layer.compare("roads") == 0) {
        for(auto prop : _props.stringProps) {
            if(prop.first.compare("name") == 0) {
                fsuint textId;

                glfonsGenText(m_fontContext, 1, &textId);
                glfonsRasterize(m_fontContext, textId, prop.second.c_str(), FONS_EFFECT_NONE);
            }
        }
    }

    std::vector<float> vertData;

    if(glfonsVertices(m_fontContext, &vertData)) {
        _mesh.addVertices(reinterpret_cast<GLbyte*>(&vertData[0]), vertData.size());
    }
    
}

void FontStyle::buildPolygon(Polygon& _polygon, std::string& _layer, Properties& _props, VboMesh& _mesh) {

}

void FontStyle::prepareDataProcessing(MapTile& _tile) {
    m_buildMutex.lock();

    m_processedTile = &_tile;

    fsuint buffer;
    
    glfonsBufferCreate(m_fontContext, 32, &buffer);
    _tile.setTextBuffer(buffer);
    glfonsBindBuffer(m_fontContext, buffer);
}

void FontStyle::finishDataProcessing(MapTile& _tile) {
    glfonsBindBuffer(m_fontContext, 0);

    m_processedTile = nullptr;

    m_buildMutex.unlock();
}

void FontStyle::setup() {
    while(m_pendingTileTexTransforms.size() > 0) {
        logMsg("create tex transforms\n");
        std::pair<TileID, glm::vec2> pair = m_pendingTileTexTransforms.top();

        glm::vec2 size = pair.second;

        GLuint texTransform;
        glGenTextures(1, &texTransform);
        glBindTexture(GL_TEXTURE_2D, texTransform);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        m_tileTexTransforms.insert(std::pair<TileID, GLuint>(pair.first, texTransform));

        m_pendingTileTexTransforms.pop();
    }

    while(m_pendingTexTransformsData.size() > 0) {
        logMsg("update transforms");
        TileTexDataTransform transformData = m_pendingTexTransformsData.top();

        glBindTexture(GL_TEXTURE_2D, m_tileTexTransforms[transformData.m_id]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, transformData.m_xoff, transformData.m_yoff,
                        transformData.m_width, transformData.m_height, GL_RGBA, GL_UNSIGNED_BYTE, transformData.m_pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_pendingTexTransformsData.pop();
    }
}

void createTexTransforms(void* _userPtr, unsigned int _width, unsigned int _height) {
    FontStyle* fontStyle = static_cast<FontStyle*>(_userPtr);

    glm::vec2 size = glm::vec2(_width, _height);

    fontStyle->m_pendingTileTexTransforms.push(std::pair<TileID, glm::vec2>(fontStyle->m_processedTile->getID(), size));
}

void updateTransforms(void* _userPtr, unsigned int _xoff, unsigned int _yoff,
                      unsigned int _width, unsigned int _height, const unsigned int* _pixels) {
    FontStyle* fontStyle = static_cast<FontStyle*>(_userPtr);

    fontStyle->m_pendingTexTransformsData.push({
        fontStyle->m_processedTile->getID(),
        _pixels, _xoff, _yoff, _width, _height
    });
}

void updateAtlas(void* _userPtr, unsigned int _xoff, unsigned int _yoff,
                 unsigned int _width, unsigned int _height, const unsigned int* _pixels) {
    logMsg("update atlas\n");
}

void createAtlas(void* _userPtr, unsigned int _width, unsigned int _height) {
    logMsg("create atlas");
    FontStyle* fontStyle = static_cast<FontStyle*>(_userPtr);

    glGenTextures(1, &fontStyle->m_atlas);
    glBindTexture(GL_TEXTURE_2D, fontStyle->m_atlas);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _width, _height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FontStyle::initFontContext(const std::string& _fontFile) {
    GLFONSparams params;

    params.createAtlas = createAtlas;
    params.createTexTransforms = createTexTransforms;
    params.updateAtlas = updateAtlas;
    params.updateTransforms = updateTransforms;

    m_fontContext = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT, params, (void*) this);

    unsigned int dataSize;
    unsigned char* data = bytesFromResource(_fontFile.c_str(), &dataSize);
    m_font = fonsAddFont(m_fontContext, "droid-serif", data, dataSize);

    if(m_font == FONS_INVALID) {
        logMsg("Error loading font file %s\n", _fontFile.c_str());
    }

}
