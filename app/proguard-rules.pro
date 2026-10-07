-keep class com.aether3d.studio.MainActivity {
    *;
}



-keepclassmembers class com.aether3d.studio.MainActivity$NativePhysicsBridge {
    public <methods>;
}



-keepclasseswithmembers,allowoptimization class * {
    native <methods>;
}



-keepattributes JavascriptInterface



-dontwarn androidx.webkit.**

