#include "tool_config.h"


#include "command.h"
#include "constants.h"

#include <algorithm>
#include <memory>


ToolConfig::ToolConfig() {
    //FUTURE: load from persistent source (document file, or user profile dir)
    
    for (auto cmdType: {
        CommandType::Select,
        CommandType::Draw,
        CommandType::Fill,
        CommandType::Erase,
        CommandType::Scroll,
        CommandType::Zoom,
    }) {
        
        Command *cmdConfig;
        
        switch (cmdType) {
            case Select:
                cmdConfig = new CommandSelect {};
                break;

            case Draw: 
                cmdConfig = new CommandDraw {Qt::black, defaultDrawWidth };
                break;
            case Fill:
                cmdConfig = new CommandFill {Qt::black, QPoint{} };
                break;
            case Erase:
                cmdConfig = new CommandErase { defaultDrawWidth * 4 };
                break;

            case Scroll:
                cmdConfig = new CommandScroll {  };
                break;

            case Zoom:
                cmdConfig = new CommandZoom {  };
                break;
            
            default:
                throw "Not a configurable command";
        }
        
        m_configs.insert(std::make_pair(cmdType, std::unique_ptr<Command>(cmdConfig)));
    }
}


std::unique_ptr<Command> ToolConfig::createCommand(CommandType cmdType) {
    return m_configs[cmdType]->clone();
}

Command * ToolConfig::getConfig(CommandType commandType) {
    return m_configs[commandType].get();
}

// void ToolConfig::updateConfig(Command * command) {
//     m_configs.insert(std::make_pair(command->type(), command->clone()));
// }
