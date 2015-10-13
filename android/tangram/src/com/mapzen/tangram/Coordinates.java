/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.3
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.mapzen.tangram;

public class Coordinates {
    private long swigCPtr;
    protected boolean swigCMemOwn;

    protected Coordinates(long cPtr, boolean cMemoryOwn) {
        swigCMemOwn = cMemoryOwn;
        swigCPtr = cPtr;
    }

    protected static long getCPtr(Coordinates obj) {
        return (obj == null) ? 0 : obj.swigCPtr;
    }

    protected void finalize() {
        delete();
    }

    public synchronized void delete() {
        if (swigCPtr != 0) {
            if (swigCMemOwn) {
                swigCMemOwn = false;
                TangramJNI.delete_Coordinates(swigCPtr);
            }
            swigCPtr = 0;
        }
    }

    // Ensure that the GC doesn't collect any Polygon instance set from Java
    private Object owner;

    protected void addReference(Object obj) {
        owner = obj;
    }

    public Coordinates() {
        this(TangramJNI.new_Coordinates__SWIG_0(), true);
    }

    public Coordinates(long n) {
        this(TangramJNI.new_Coordinates__SWIG_1(n), true);
    }

    public long size() {
        return TangramJNI.Coordinates_size(swigCPtr, this);
    }

    public long capacity() {
        return TangramJNI.Coordinates_capacity(swigCPtr, this);
    }

    public void reserve(long n) {
        TangramJNI.Coordinates_reserve(swigCPtr, this, n);
    }

    public boolean isEmpty() {
        return TangramJNI.Coordinates_isEmpty(swigCPtr, this);
    }

    public void clear() {
        TangramJNI.Coordinates_clear(swigCPtr, this);
    }

    public void add(LngLat x) {
        TangramJNI.Coordinates_add__SWIG_0(swigCPtr, this, LngLat.getCPtr(x), x);
    }

    public LngLat get(int i) {
        long cPtr = TangramJNI.Coordinates_get(swigCPtr, this, i);
        LngLat ret = null;
        if (cPtr != 0) {
            ret = new LngLat(cPtr, false);
            ret.addReference(this);
        }
        return ret;
    }

    public void set(int i, LngLat val) {
        TangramJNI.Coordinates_set(swigCPtr, this, i, LngLat.getCPtr(val), val);
    }

    public void add(double lng, double lat) {
        TangramJNI.Coordinates_add__SWIG_1(swigCPtr, this, lng, lat);
    }

}
