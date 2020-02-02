#pragma once

#include <exception>
#include <string>

using namespace std;

struct not_found_exception : public runtime_error {
    explicit not_found_exception(const string &name) : runtime_error("Object \"" + name + "\" not found") {}
};

struct no_program_exception : public logic_error {
    explicit no_program_exception() : logic_error("Program has not been read correctly") {}
};

struct invalid_shader_exception : public logic_error {
    explicit invalid_shader_exception() : logic_error("Invalid Shader Program") {};
};