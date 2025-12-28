#include "hook.h"
#include "logging.h"
#include <string>

using namespace std;

jstring (*orig_native_get)(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ);

jstring my_native_get(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ) {
    const char *key = env->GetStringUTFChars(keyJ, nullptr);
    const char *def = env->GetStringUTFChars(defJ, nullptr);

    jstring hooked_result = nullptr;

    if (strcmp(key, "ro.product.brand") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.manufacturer") == 0) {
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.model") == 0) {
        hooked_result = env->NewStringUTF("23127PN0CC");
    } else if (strcmp(key, "ro.build.version.incremental") == 0) {
        hooked_result = env->NewStringUTF("V14.0.6.0.UMCCNXM");
    } else if (strcmp(key, "ro.miui.ui.version.code") == 0) {
        hooked_result = env->NewStringUTF("14");
    } else if (strcmp(key, "ro.miui.ui.version.name") == 0) {
        hooked_result = env->NewStringUTF("V14");
    } else if (strcmp(key, "ro.miui.version.code_time") == 0) {
        hooked_result = env->NewStringUTF("230618");
    } else if (strcmp(key, "ro.product.device") == 0) {
        hooked_result = env->NewStringUTF("shennong");
    } else if (strcmp(key, "ro.product.name") == 0) {
        hooked_result = env->NewStringUTF("shennong");
    }

    env->ReleaseStringUTFChars(keyJ, key);
    env->ReleaseStringUTFChars(defJ, def);

    if (hooked_result != nullptr) {
        return hooked_result;
    } else {
        return orig_native_get(env, clazz, keyJ, defJ);
    }
}

void hookBuild(JNIEnv *env) {
    LOGD("hook Build\n");
    jclass build_class = env->FindClass("android/os/Build");
    jstring new_brand = env->NewStringUTF("Xiaomi");
    jstring new_manufacturer = env->NewStringUTF("Xiaomi");
    jstring new_model = env->NewStringUTF("23127PN0CC");
    jstring new_device = env->NewStringUTF("shennong");
    jstring new_product = env->NewStringUTF("shennong");
    
    // 修改 BRAND
    jfieldID brand_id = env->GetStaticFieldID(build_class, "BRAND", "Ljava/lang/String;");
    if (brand_id != nullptr) {
        env->SetStaticObjectField(build_class, brand_id, new_brand);
    }

    // 修改 MANUFACTURER
    jfieldID manufacturer_id = env->GetStaticFieldID(build_class, "MANUFACTURER", "Ljava/lang/String;");
    if (manufacturer_id != nullptr) {
        env->SetStaticObjectField(build_class, manufacturer_id, new_manufacturer);
    }
    
    // 修改 MODEL
    jfieldID model_id = env->GetStaticFieldID(build_class, "MODEL", "Ljava/lang/String;");
    if (model_id != nullptr) {
        env->SetStaticObjectField(build_class, model_id, new_model);
    }
    
    // 修改 DEVICE
    jfieldID device_id = env->GetStaticFieldID(build_class, "DEVICE", "Ljava/lang/String;");
    if (device_id != nullptr) {
        env->SetStaticObjectField(build_class, device_id, new_device);
    }
    
    // 修改 PRODUCT
    jfieldID product_id = env->GetStaticFieldID(build_class, "PRODUCT", "Ljava/lang/String;");
    if (product_id != nullptr) {
        env->SetStaticObjectField(build_class, product_id, new_product);
    }

    env->DeleteLocalRef(new_brand);
    env->DeleteLocalRef(new_manufacturer);
    env->DeleteLocalRef(new_model);
    env->DeleteLocalRef(new_device);
    env->DeleteLocalRef(new_product);

    LOGD("hook Build done");
}

void hookSystemProperties(JNIEnv *env, zygisk::Api *api) {
    LOGD("hook SystemProperties\n");

    JNINativeMethod targetHookMethods[] = {
            {"native_get", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
             (void *) my_native_get},
    };

    api->hookJniNativeMethods(env, "android/os/SystemProperties", targetHookMethods, 1);

    *(void **) &orig_native_get = targetHookMethods[0].fnPtr;

    LOGD("hook SystemProperties done: %p\n", orig_native_get);
}

void Hook::hook() {
    hookBuild(env);
    hookSystemProperties(env, api);
}
