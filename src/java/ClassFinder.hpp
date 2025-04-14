#pragma once
#include "pch.hpp"

#include "JavaException.hpp"

#include "jni.h"
#include "jvmti.h"

class ClassFinder {
public:
	explicit ClassFinder(JNIEnv* _env, jvmtiEnv* _tienv);

	void findClasses();

	jclass getClass(const std::string& name) const;

	void dumpClassFile(const std::string& name) const;

	void writeClassNamesToFile(const std::string& fileName = "classes.txt") const;

	void releaseClasses();

	~ClassFinder();

private:
	JNIEnv* env;
	jvmtiEnv* tienv;

	std::map<std::string, jclass> m_classMap;
};
