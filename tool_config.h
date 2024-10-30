#ifndef TOOL_CONFIG_H
#define TOOL_CONFIG_H


#include "command.h"
#include <memory>


class ToolConfig
{
public:
    static ToolConfig& instance() {
        static ToolConfig instance;

        return instance;
    }

    std::unique_ptr<Command> createCommand(CommandType commandType);
    Command * getConfig(CommandType commandType);

private:
    std::map<CommandType, std::unique_ptr<Command>> m_configs;

    ToolConfig();
    ~ToolConfig() = default;
};


#endif // TOOL_CONFIG_H
