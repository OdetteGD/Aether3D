#include <jni.h>
#include <android/log.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
namespace{constexpr const char*kTag="Aether3D";float fast_inverse_square_root(float n)noexcept{if(!(n>0.0f)||!std::isfinite(n))return std::numeric_limits<float>::quiet_NaN();const float x2=n*.5F;float y=n;std::uint32_t b=0;std::memcpy(&b,&y,sizeof(y));b=0x5f3759dfU-(b>>1U);std::memcpy(&y,&b,sizeof(y));return y*(1.5F-(x2*y*y));}jstring engine_version(JNIEnv*e){return e->NewStringUTF("Aether3D Native Core 1.0.0 / C++20 / NDK");}void log_call(const char*n){__android_log_print(ANDROID_LOG_DEBUG,kTag,"%s",n);}}
extern "C" JNIEXPORT jfloat JNICALL Java_com_aether3d_studio_MainActivity_fastInverseSquareRoot(JNIEnv*e,jclass,jfloat v){(void)e;log_call("MainActivity.fastInverseSquareRoot");return fast_inverse_square_root(v);}
extern "C" JNIEXPORT jstring JNICALL Java_com_aether3d_studio_MainActivity_getEngineVersion(JNIEnv*e,jclass){log_call("MainActivity.getEngineVersion");return engine_version(e);}
extern "C" JNIEXPORT jfloat JNICALL Java_com_aether3d_studio_Aether3DNativeBridge_fastInverseSquareRoot(JNIEnv*e,jclass,jfloat v){(void)e;return fast_inverse_square_root(v);}
extern "C" JNIEXPORT jstring JNICALL Java_com_aether3d_studio_Aether3DNativeBridge_getEngineVersion(JNIEnv*e,jclass){return engine_version(e);}
