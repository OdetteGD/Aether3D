package com.aether3d.studio;
public final class Aether3DNativeBridge{static{System.loadLibrary("aether3d_core");}private Aether3DNativeBridge(){}public static native float fastInverseSquareRoot(float value);public static native String getEngineVersion();}
