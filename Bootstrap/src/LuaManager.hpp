#pragma once
#include <sol/sol.hpp>
#include <string>
#include <Util/Log.hpp>
#include <functional>

class LuaTableRegistrar {
public:
    LuaTableRegistrar(sol::state& lua, const std::string& tableName)
        : lua(lua), tbl(lua.create_table()), tableName(tableName) {}

    template<typename Func>
    LuaTableRegistrar& RegisterFunction(const std::string& name, Func&& f) {
        tbl[name] = std::forward<Func>(f);
        return *this;
    }

    template<typename T>
    LuaTableRegistrar& RegisterVariable(const std::string& name, T&& value) {
        tbl[name] = std::forward<T>(value);
        return *this;
    }

    ~LuaTableRegistrar() {
        lua[tableName] = tbl;
    }

private:
    sol::state& lua;
    sol::table tbl;
    std::string tableName;
};

class LuaManager {
public:
    LuaManager() {
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);
    }

    bool LoadScript(const std::string& path) {
        sol::load_result script = lua.load_file(path);
        if (!script.valid()) {
            sol::error err = script;
            Util::Logger("LuaManager").Error("Failed to load Lua script '{}': {}", path, err.what());
            return false;
        }
        sol::protected_function_result result = lua.script_file(path);
        if (!result.valid()) {
            sol::error err = result;
            Util::Logger("LuaManager").Error("Error executing Lua script '{}': {}", path, err.what());
            return false;
        }
        return true;
    }

    bool LoadScriptFromMemory(const std::string& code, const std::string& chunkName = "chunk") {
        sol::load_result script = lua.load(code, chunkName);
        if (!script.valid()) {
            sol::error err = script;
            Util::Logger("LuaManager").Error("Failed to load Lua script from memory '{}': {}", chunkName, err.what());
            return false;
        }
        sol::protected_function_result result = script();
        if (!result.valid()) {
            sol::error err = result;
            Util::Logger("LuaManager").Error("Error executing Lua script from memory '{}': {}", chunkName, err.what());
            return false;
        }
        return true;
    }

    template<typename... Args>
    void Call(const std::string& funcName, Args&&... args) {
        sol::function func = lua[funcName];
        if (!func.valid()) {
            Util::Logger("LuaManager").Warn("Lua function '{}' does not exist", funcName);
            return;
        }
        sol::protected_function_result result = func(std::forward<Args>(args)...);
        if (!result.valid()) {
            sol::error err = result;
            Util::Logger("LuaManager").Error("Lua error in function '{}': {}", funcName, err.what());
            std::terminate();
        }
    }

    LuaTableRegistrar RegisterTable(const std::string& tableName) {
        return LuaTableRegistrar(lua, tableName);
    }

    sol::table CreateRawTable() {
        return lua.create_table();
    }

    bool GetBool(const std::string& dottedPath, bool fallback = false) {
        auto obj = GetNestedObject(dottedPath);
        if (obj.is<bool>()) return obj.as<bool>();
        if (obj.is<int>()) return obj.as<int>() != 0;
        return fallback;
    }

    int GetInt(const std::string& dottedPath, int fallback = 0) {
        auto obj = GetNestedObject(dottedPath);
        if (obj.is<int>()) return obj.as<int>();
        if (obj.is<double>()) return static_cast<int>(obj.as<double>());
        if (obj.is<bool>()) return obj.as<bool>() ? 1 : 0;
        return fallback;
    }

    double GetDouble(const std::string& dottedPath, double fallback = 0.0) {
        auto obj = GetNestedObject(dottedPath);
        if (obj.is<double>()) return obj.as<double>();
        if (obj.is<int>()) return static_cast<double>(obj.as<int>());
        return fallback;
    }

    std::string GetString(const std::string& dottedPath, const std::string& fallback = "") {
        auto obj = GetNestedObject(dottedPath);
        if (obj.is<std::string>()) return obj.as<std::string>();
        return fallback;
    }

    void SetBool(const std::string& dottedPath, bool value) { SetNested(dottedPath, value); }
    void SetInt(const std::string& dottedPath, int value) { SetNested(dottedPath, value); }
    void SetDouble(const std::string& dottedPath, double value) { SetNested(dottedPath, value); }
    void SetString(const std::string& dottedPath, const std::string& value) { SetNested(dottedPath, value); }

    sol::state& GetState() { return lua; }

private:
    sol::state lua;

    sol::object GetNestedObject(const std::string& dottedPath) {
        sol::object obj = lua.globals();
        size_t start = 0, end;
        while ((end = dottedPath.find('.', start)) != std::string::npos) {
            std::string part = dottedPath.substr(start, end - start);
            if (obj.get_type() == sol::type::table) {
                obj = obj.as<sol::table>()[part];
            } else {
                return sol::nil;
            }
            start = end + 1;
        }
        std::string lastPart = dottedPath.substr(start);
        if (obj.get_type() == sol::type::table) {
            return obj.as<sol::table>()[lastPart];
        } else {
            return sol::nil;
        }
    }

    template<typename T>
    void SetNested(const std::string& dottedPath, T&& value) {
        sol::object obj = lua.globals();
        size_t start = 0, end;
        while ((end = dottedPath.find('.', start)) != std::string::npos) {
            std::string part = dottedPath.substr(start, end - start);
            if (obj.get_type() == sol::type::table) {
                obj = obj.as<sol::table>()[part];
            } else {
                obj = lua.create_table();
                lua[part] = obj;
            }
            start = end + 1;
        }
        std::string lastPart = dottedPath.substr(start);
        if (obj.get_type() == sol::type::table) {
            obj.as<sol::table>()[lastPart] = std::forward<T>(value);
        }
    }
};