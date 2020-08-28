#include "Debugger.hpp"

namespace Falcon
{
    Debugger::Debugger(uint8_t * code, const DebuggerData & debugData, PrintVarFunction printVarFunction)
        : VM(code), m_DebugData(debugData), m_Next(false), m_Continue(false), m_Finish(false), m_IC(0), m_PrintVarFunction(printVarFunction)
    {
        disassemble();
    }

    void Debugger::disassemble()
    {
        uint64_t ip = 0;

        while (m_Code[ip] != 255)
        {
            uint64_t currentIP = ip;
            OpCode::OpCode op = (OpCode::OpCode)m_Code[ip];
            OpCode::Layout layout = OpCode::s_Layouts[m_Code[ip]];

            if (layout == OpCode::Layout::BASE)
            {
                m_Disassembly[ip] = OpCode::s_Names[m_Code[ip]];
            }
            else if (layout == OpCode::Layout::UN_REG)
            {
                m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(m_Code[++ip]);
            }
            else if (layout == OpCode::Layout::UN_NUM)
            {
                if (op == OpCode::PSHNUL || op == OpCode::POPNUL || op == OpCode::RET)
                {
                    uint32_t num = *(uint32_t *)&m_Code[++ip];
                    ip += 3;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + std::to_string(num);
                }
                else
                {
                    uint64_t num = *(uint64_t *)&m_Code[++ip];
                    ip += 7;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + std::to_string(num);
                }
            }
            else if (layout == OpCode::Layout::UN_STR)
            {
                std::string str;

                char c = 0;

                ip++;

                while ((c = (char)m_Code[ip]))
                {
                    str += c;

                    ip++;
                }

                m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + str;
                ip++;
            }
            else if (layout == OpCode::Layout::BIN_REG_REG)
            {
                m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(m_Code[ip + 1]) + " " + disassembleRegister(m_Code[ip + 2]);
                ip += 2;
            }
            else if (layout == OpCode::Layout::BIN_MOV)
            {
                uint8_t reg = m_Code[++ip];

                if (op == OpCode::UMOV8)
                {
                    uint8_t num = m_Code[++ip];

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(+num);
                }
                else if (op == OpCode::IMOV8)
                {
                    int8_t num = (int8_t)m_Code[++ip];

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(+num);
                }
                else if (op == OpCode::UMOV16)
                {
                    uint16_t num = *(uint16_t *)&m_Code[++ip];
                    ip++;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
                else if (op == OpCode::IMOV16)
                {
                    int16_t num = *(int16_t *)&m_Code[++ip];
                    ip++;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
                else if (op == OpCode::UMOV32 || (op >= OpCode::LOAD8 && op <= OpCode::LODREF))
                {
                    uint32_t num = *(uint32_t *)&m_Code[++ip];
                    ip += 3;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
                else if (op == OpCode::IMOV32)
                {
                    int32_t num = *(int32_t *)&m_Code[++ip];
                    ip += 3;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
                else if (op == OpCode::FMOV32)
                {
                    float num = *(float *)&m_Code[++ip];
                    ip += 3;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
                else if (op == OpCode::UMOV64)
                {
                    uint64_t num = *(uint64_t *)&m_Code[++ip];
                    ip += 7;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
                else if (op == OpCode::IMOV64)
                {
                    int64_t num = *(int64_t *)&m_Code[++ip];
                    ip += 7;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
                else if (op == OpCode::FMOV64)
                {
                    double num = *(double *)&m_Code[++ip];
                    ip += 7;

                    m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + disassembleRegister(reg) + " " + std::to_string(num);
                }
            }
            else if (layout == OpCode::Layout::BIN_CALL)
            {
                uint32_t num = *(uint32_t *)&m_Code[++ip];
                ip += 4;

                std::string str;

                char c = 0;

                while ((c = (char)m_Code[ip]))
                {
                    str += c;

                    ip++;
                }

                m_Disassembly[currentIP] = OpCode::s_Names[m_Code[currentIP]] + " " + std::to_string(num) + " " + str;
            }

            ip++;
        }

        for (auto iter = m_Disassembly.begin(); iter != m_Disassembly.end(); iter++)
        {
            for (char & c : iter->second)
            {
                c = std::tolower(c);
            }
        }
    }

    std::string Debugger::disassembleRegister(uint8_t reg)
    {
        std::string regStr;

        if (reg & 0b00001000)
        {
            regStr = "[" + RegisterType::s_Names[reg & 0b00000011] + "]";
        }
        else if (reg & 0b00000100)
        {
            regStr = "@" + RegisterType::s_Names[reg & 0b00000011];
        }
        else
        {
            regStr = RegisterType::s_Names[reg];
        }

        return regStr;
    }

    void Debugger::updateDebuggerState()
    {
        uint64_t i = 0;

        for (auto & inst : m_Disassembly)
        {
            if (inst.first == m_IP)
            {
                m_IC = i;
                break;
            }

            i++;
        }

        for (auto iter = m_DebugData.LineData.begin(); iter != m_DebugData.LineData.end(); iter++)
        {
            if (iter->first > m_IC)
            {
                iter--;

                m_LC = iter->second.first;
                m_CurrentLine = iter->second.second;
                break;
            }
            else if (iter->first == m_IC)
            {
                m_LC = iter->second.first;
                m_CurrentLine = iter->second.second;
                break;
            }
        }

        for (auto & function : m_DebugData.FunctionData)
        {
            if (m_LC >= function.second.StartLine && m_LC <= function.second.EndLine)
            {
                m_CurrentFunction = function.first;
            }
        }
    }

    void Debugger::shell()
    {
        while (true)
        {
            std::cout<<"> ";

            std::string cmd;

            std::cin>>cmd;

            if (cmd == "n" || cmd == "next")
            {
                m_Next = true;
                return;
            }
            else if (cmd == "c" || cmd == "continue")
            {
                m_Continue = true;
                return;
            }
            else if (cmd == "f" || cmd == "finish")
            {
                m_Finish = true;
                return;
            }
            else if (cmd == "q" || cmd == "quit")
            {
                m_Running = false;
                return;
            }
            else if (cmd == "b" || cmd == "breakpoint")
            {
                uint64_t location;

                std::cin>>location;

                setBreakpoint(location);
            }
            else if (cmd == "cl" || cmd == "clear")
            {
                uint64_t location;

                std::cin>>location;

                clearBreakpoint(location);
            }
            else if (cmd == "r" || cmd == "registers")
            {
                for (uint8_t i = 0; i < RegisterType::s_Names.size(); i++)
                {
                    Register & reg = getRegister((RegisterType::RegisterType)i);

                    std::cout<<"\033[0;1;33m"<<RegisterType::s_Names[i]<<":\033[0;0m\n";
                    std::cout<<"    u8:  "<<+reg.u8<<"\tu16: "<<reg.u16<<"\tu32: "<<reg.u32<<"\tu64: "<<reg.u64<<"\n\n";
                    std::cout<<"    i8:  "<<+reg.i8<<"\ti16: "<<reg.i16<<"\ti32: "<<reg.i32<<"\ti64: "<<reg.i64<<"\n\n";
                    std::cout<<"    f32: "<<reg.f32<<"\tf64: "<<reg.f64<<"\n";
                }
            }
            else if (cmd == "stk" || cmd == "stack")
            {
                uint64_t size;

                std::cin>>size;

                uint64_t lineCount = size / 16;

                std::cout<<"\033[0;1;33mStacktrace:\n";
                
                uint64_t offset = m_FP;

                for (int i = 0; i < lineCount; i++)
                {
                    std::cout<<"    \033[0;1;34m0x"<<std::hex<<std::setfill('0')<<std::setw(6)<<offset<<std::dec<<std::setw(0)<<"\033[0;0m ";

                    offset += 16;
                    
                    for (int j = 0; j < 16; j++)
                    {
                        std::cout<<"0x"<<std::hex<<std::setfill('0')<<std::setw(2)<<+(m_Stack[m_FP + i * 16 + j])<<std::dec<<std::setw(0)<<(j < 15 ? " " : "\n");
                    }
                }

                if (size % 16)
                {
                    uint64_t rem = size % 16;

                    std::cout<<"    \033[0;1;34m0x"<<std::hex<<std::setfill('0')<<std::setw(6)<<offset<<std::dec<<std::setw(0)<<"\033[0;0m ";

                    for (int i = 0; i < rem; i++)
                    {
                        std::cout<<"0x"<<std::hex<<std::setfill('0')<<std::setw(2)<<+(m_Stack[m_FP + lineCount * 16 + i])<<std::dec<<std::setw(0)<<(i < rem - 1 ? " " : "\n");
                    }
                }
            }
            else if (cmd == "stt" || cmd == "state")
            {
                std::cout<<"\033[0;1;33mIC: \033[0;1;34m0x"<<std::hex<<m_IC<<"\033[0;0m "<<std::dec<<m_IC<<".\n";
                std::cout<<"\033[0;1;33mIP: \033[0;1;34m0x"<<std::hex<<m_IP<<"\033[0;0m "<<std::dec<<m_IP<<".\n";
                std::cout<<"\033[0;1;33mSP: \033[0;1;34m0x"<<std::hex<<m_SP<<"\033[0;0m "<<std::dec<<m_SP<<".\n";
                std::cout<<"\033[0;1;33mFP: \033[0;1;34m0x"<<std::hex<<m_FP<<"\033[0;0m "<<std::dec<<m_FP<<".\n";
                std::cout<<"\033[0;1;33mFLAGS: \033[0;0m{ "<<(m_Cmp[0] ? "true" : "false")<<", "<<(m_Cmp[1] ? "true" : "false")<<" }.\n";
            }
            else if (cmd == "dsasm" || cmd == "disassembly")
            {
                std::cout<<"\033[0;1;33m"<<"Disassembly:"<<"\033[0;0m\n";

                auto tmp = m_Disassembly.emplace(m_IP, "").first;

                uint64_t begin, end;

                for (auto iter = m_Disassembly.find(m_IP); true; iter--)
                {
                    if (iter->second == "func")
                    {
                        begin = iter->first;
                        break;
                    }

                    if (iter == m_Disassembly.begin())
                    {
                        break;
                    }
                }

                for (auto iter = m_Disassembly.find(m_IP); iter != m_Disassembly.end(); iter++)
                {
                    if (iter->second == "func")
                    {
                        end = iter->first;
                        break;
                    }
                }

                if (tmp->second == "")
                {
                    m_Disassembly.erase(tmp);
                }

                for (auto iter = m_Disassembly.find(begin); iter != m_Disassembly.find(end); iter++)
                {
                    if (iter->second == "func")
                    {
                        for (auto & f : m_Functions)
                        {
                            if (f.second == iter->first)
                            {
                                std::cout<<"    \033[0;32m["<<f.first<<"]\033[0;0m\n";
                            }
                        }
                    }

                    if (iter->first == m_IP)
                    {
                        std::cout<<" \033[0;1;31m>>\033[0;0m ";
                    }
                    else
                    {
                        std::cout<<"    ";
                    }

                    std::cout<<"+["<<iter->first<<"]\t"<<iter->second<<"\n";
                }
            }
            else if (cmd == "p" || cmd == "print")
            {
                std::string var;

                std::cin>>var;

                auto & data = m_DebugData.FunctionData[m_CurrentFunction];

                if (data.LocalVariables.count(var) == 1)
                {
                    if (data.LocalVariables[var].second < m_SP)
                    {
                        if (m_PrintVarFunction)
                        {
                            std::cout<<"'"<<var<<"' = "<<m_PrintVarFunction(data.LocalVariables[var].first, (void *)&m_Stack[m_FP + data.LocalVariables[var].second])<<"\n";
                        }
                        else
                        {
                            std::cout<<"No `PrintVarFunction` defined.\n";
                        }
                    }
                    else
                    {
                        std::cout<<"Variable '"<<var<<"' is not defined.\n";
                    }
                }
                else
                {
                    std::cout<<"Variable '"<<var<<"' is not defined.\n";
                }
            }
        }
    }

    void Debugger::setBreakpoint(uint64_t pos)
    {
        if (std::find(m_Breakpoints.begin(), m_Breakpoints.end(), pos) == m_Breakpoints.end())
        {
            m_Breakpoints.emplace_back(pos);
        }
        else
        {
            std::cout<<"Breakpoint "<<m_Breakpoints.size()<<" already at Instruction Counter: "<<std::hex<<"\033[0;1;34m0x"<<pos<<std::dec<<"\033[0;0m.\n";
        }

        std::cout<<"Breakpoint "<<m_Breakpoints.size()<<" set at Instruction Counter: "<<std::hex<<"\033[0;1;34m0x"<<pos<<std::dec<<"\033[0;0m.\n";
    }

    void Debugger::clearBreakpoint(uint64_t pos)
    {
        auto iter = m_Breakpoints.end();

        if ((iter = std::find(m_Breakpoints.begin(), m_Breakpoints.end(), pos)) != m_Breakpoints.end())
        {
            m_Breakpoints.erase(iter);
            std::cout<<"Cleared breakpoint "<<m_Breakpoints.size()<<" at Instruction Counter: "<<std::hex<<"\033[0;1;34m0x"<<pos<<std::dec<<"\033[0;0m.\n";
        }
        else
        {
            std::cout<<"No breakpoint set at Instruction Counter: "<<std::hex<<"\033[0;1;34m0x"<<pos<<std::dec<<"\033[0;0m.\n";
        }
    }

    void Debugger::run(const std::string & function, uint64_t argsSize)
    {
        m_IP = m_Functions[function] + 1;

        uint8_t * args = new uint8_t[argsSize];

        memcpy(args, pop(argsSize), argsSize);

        uint64_t returnIP = 0;

        push((uint8_t *)&m_SP, 8);
        push((uint8_t *)&returnIP, 8);
        push((uint8_t *)&m_FP, 8);

        m_FP = m_SP;

        push(args, argsSize);

        delete[] args;

        updateDebuggerState();

        m_Running = true;
        OpCode::OpCode op = (OpCode::OpCode)m_Code[m_IP];

        while (m_Running)
        {
            if (std::find(m_Breakpoints.begin(), m_Breakpoints.end(), m_IC) != m_Breakpoints.end())
            {
                m_Continue = false;
                std::cout<<"Breakpoint hit at Instruction Counter \033[0;1;34m0x"<<std::hex<<m_IC<<std::dec<<"\033[0;0m:\n";
                std::cout<<"    \033[0;1;33mLine "<<m_LC<<": \033[0;0m"<<m_CurrentLine<<"\n";
                shell();

                if (!m_Running)
                {
                    break;
                }
            }

            if (m_Next)
            {
                m_Next = false;
                
                uint64_t currentLC = m_LC;

                while (m_LC == currentLC)
                {
                    (this->*m_Operators[(uint8_t)op])();
                    
                    op = (OpCode::OpCode)m_Code[++m_IP];

                    updateDebuggerState();
                }

                std::cout<<"\033[0;1;33mLine "<<m_LC<<": \033[0;0m"<<m_CurrentLine<<"\n";

                shell();
                
                if (!m_Running)
                {
                    break;
                }
            }

            (this->*m_Operators[(uint8_t)op])();

            if (m_Finish)
            {
                if (op == OpCode::RET)
                {
                    m_Finish = false;

                    std::cout<<"Finished.";
                    shell();

                    if (!m_Running)
                    {
                        break;
                    }
                }
            }

            op = (OpCode::OpCode)m_Code[++m_IP];

            updateDebuggerState();
        }
    }
}