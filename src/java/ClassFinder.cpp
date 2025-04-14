// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "pch.hpp"
#include "ClassFinder.hpp"

static void JNICALL cflhDumpClassFile(
	jvmtiEnv* jvmti_env,
	JNIEnv* jni_env,
	jclass class_being_redefined,
	jobject loader,
	const char* name,
	jobject protection_domain,
	jint class_data_len,
	const uint8_t* class_data,
	jint* new_class_data_len,
	uint8_t** new_class_data
) {
	std::ofstream out;
	std::string fileName = name;
	for (char& c : fileName) if (c == '/') c = '_';
	fileName.append(".class");
	out.open(fileName, std::ios::binary | std::ios::out);
	out << class_data;
	out.close();
}

ClassFinder::ClassFinder(JNIEnv* _env, jvmtiEnv* _tienv) 
	: env(_env), tienv(_tienv) { }

void ClassFinder::findClasses() {
	jint iClassCount = 0;
	jclass* oClassArray = nullptr;
	jvmtiError e = tienv->GetLoadedClasses(&iClassCount, &oClassArray);
	if (e != JVMTI_ERROR_NONE)
		throw JavaException("Could not get all of the loaded classes (jvmti error code " + std::to_string(e) + ")");

	jclass cClass = env->FindClass("java/lang/Class");
	if (cClass == nullptr)
		throw JavaException("Could not find the Class class");

	jmethodID mGetName = env->GetMethodID(cClass, "getName", "()Ljava/lang/String;");
	if (mGetName == nullptr)
		throw JavaException("Could not find the Class.getName method");

	for (int i = 0; i < iClassCount; i++) {
		jclass oClass = oClassArray[i];

		jstring oClassName = static_cast<jstring>(env->CallObjectMethod(oClass, mGetName));

		const char* classNamePtr = env->GetStringUTFChars(oClassName, nullptr);

		std::string classNameString(classNamePtr);

		for (char& c : classNameString) if (c == '.') c = '/';

		m_classMap.emplace(classNameString, oClass);

		env->ReleaseStringUTFChars(oClassName, classNamePtr);
	}
}

jclass ClassFinder::getClass(const std::string& name) const {
	if (m_classMap.count(name) == 0)
		throw JavaException("Could not find class " + name);

	return m_classMap.at(name);
}

void ClassFinder::dumpClassFile(const std::string& name) const {
	jclass clazz = getClass(name);

	jvmtiEventCallbacks callbacks{
		.ClassFileLoadHook = cflhDumpClassFile
	};

	if (tienv->SetEventCallbacks(&callbacks, sizeof(callbacks)) != JVMTI_ERROR_NONE)
		throw JavaException("Failed to set event callbacks in dumpClassFile");

	if (tienv->RetransformClasses(1, &clazz) != JVMTI_ERROR_NONE)
		throw JavaException("Failed to retransform class " + name);

	if (tienv->SetEventCallbacks(nullptr, 0) != JVMTI_ERROR_NONE)
		throw JavaException("Failed to reset event callbacks");
}

void ClassFinder::writeClassNamesToFile(const std::string& fileName) const {
	std::ofstream out;
	out.open(fileName);
	for (const auto& [name, clazz] : m_classMap)
		out << name << '\n';
	out.close();
}

void ClassFinder::releaseClasses() {
	m_classMap.clear();
}

ClassFinder::~ClassFinder() {
	releaseClasses();
}
