/* 046267 Computer Architecture - Spring 2019 - HW #4 */

#include "core_api.h"
#include "sim_api.h"

#include <stdio.h>
#include <memory>
#include <vector>

using uint = unsigned int;

struct Command
{
    Command(const int &cycles = 0) :
        remains_cycles(cycles),
        is_done(true)
    {

    }

    Instuction data;
    int remains_cycles;
    bool is_done;
};

class Thread
{
public:
    Thread() :
        _regs(REGS),
        _commands_done(0),
        _curr_command(),
        _is_done(false),
        _is_accessed_memory_first_time(false)
    {

    }

    Thread &readNextCommand(const uint &thread_id)
    {
        if(_curr_command.remains_cycles > 0)
        {
            return *this;
        }

        if(_curr_command.is_done)
        {
            SIM_MemInstRead(_commands_done, &_curr_command.data, static_cast<int>(thread_id));
            _curr_command.remains_cycles = Thread::opcodeTime(_curr_command.data.opcode);
            _curr_command.is_done = false;
        }

        return *this;
    }

    bool isDone() const {return _is_done;}

    Thread &copyRegsToContext(tcontext *context)
    {
        for(uint i = 0; i < REGS; i++)
        {
            context->reg[i] = _regs[i];
        }

        return *this;
    }

    static int opcodeTime(const cmd_opcode &opcode)
    {
        Latency latency = {};
        Mem_latency(reinterpret_cast<int*>(&latency));

        if(opcode == CMD_LOAD)
        {
            return 1 + latency.Load_latecny;
        }
        else if(opcode == CMD_STORE)
        {
            return 1 + latency.Store_latency;
        }

        return 1;
    }

    Thread &handleCommand()
    {
        if(_curr_command.data.opcode == CMD_NOP)
        {
            return *this;
        }
        else if(_curr_command.data.opcode == CMD_ADD ||
                _curr_command.data.opcode == CMD_ADDI)
        {
            return handleAdd();
        }
        else if(_curr_command.data.opcode == CMD_SUB ||
                _curr_command.data.opcode == CMD_SUBI)
        {
            return handleSub();
        }
        else if(_curr_command.data.opcode == CMD_LOAD)
        {
            return handleLoad();
        }
        else if(_curr_command.data.opcode == CMD_STORE)
        {
            return handleStore();
        }
        else if(_curr_command.data.opcode == CMD_HALT)
        {
            return handleHalt();
        }

        return *this;
    }

    bool hasMemoryAccess() const
    {
        return _curr_command.data.opcode == CMD_LOAD || _curr_command.data.opcode == CMD_STORE;
    }

    bool isAccesssedMemoryFirstTime() const {return _is_accessed_memory_first_time && hasMemoryAccess();}

    uint getCommandsDone() const {return _commands_done;}

    Thread &tik()
    {
        if(_curr_command.remains_cycles > 0)
        {
            _curr_command.remains_cycles--;
        }
        return *this;
    }

    Command getCommand() const {return _curr_command;}

private: //methods
    Thread &handleAdd()
    {
        uint dst = static_cast<uint>(_curr_command.data.dst_index);
        uint src1 = static_cast<uint>(_curr_command.data.src1_index);
        if(!_curr_command.data.isSrc2Imm)
        {
            uint src2 = static_cast<uint>(_curr_command.data.src2_index_imm);
            _regs[dst] = _regs[src1] + _regs[src2];
        }
        else
        {
            _regs[dst] = _regs[src1] + _curr_command.data.src2_index_imm;
        }

        _curr_command.is_done = true;
        _commands_done++;

        return *this;
    }

    Thread &handleSub()
    {
        uint dst = static_cast<uint>(_curr_command.data.dst_index);
        uint src1 = static_cast<uint>(_curr_command.data.src1_index);

        if(!_curr_command.data.isSrc2Imm)
        {
            uint src2 = static_cast<uint>(_curr_command.data.src2_index_imm);
            _regs[dst] = _regs[src1] - _regs[src2];
        }
        else
        {
            _regs[dst] = _regs[src1] - _curr_command.data.src2_index_imm;
        }

        _curr_command.is_done = true;
        _commands_done++;

        return *this;
    }

    Thread &handleLoad()
    {
        _is_accessed_memory_first_time =  _curr_command.remains_cycles == Thread::opcodeTime(CMD_LOAD);

        uint dst = static_cast<uint>(_curr_command.data.dst_index);
        uint src1 = static_cast<uint>(_curr_command.data.src1_index);
        uint src2 = static_cast<uint>(_curr_command.data.src2_index_imm);

        if(_curr_command.remains_cycles == 0)
        {
            int read_val = -1;
            if(_curr_command.data.isSrc2Imm)
            {
                SIM_MemDataRead(static_cast<uint>(_regs[src1] + _curr_command.data.src2_index_imm), &read_val);
            }
            else
            {
                SIM_MemDataRead(static_cast<uint>(_regs[src1] + _regs[src2]), &read_val);
            }

            _regs[dst] = _regs[src1] + read_val;
            _curr_command.is_done = true;
            _commands_done++;
        }

        return *this;
    }

    Thread &handleStore()
    {
        uint dst = static_cast<uint>(_curr_command.data.dst_index);
        uint src1 = static_cast<uint>(_curr_command.data.src1_index);
        uint src2 = static_cast<uint>(_curr_command.data.src2_index_imm);

        if(_curr_command.remains_cycles == 0)
        {
            if(_curr_command.data.isSrc2Imm)
            {
                SIM_MemDataWrite(static_cast<uint>(static_cast<int>(_regs[dst])) + src2, _regs[src1]);
            }
            else
            {
                SIM_MemDataWrite(static_cast<uint>(_regs[dst] + _regs[src2]), _regs[src1]);
            }

            _curr_command.is_done = true;
            _commands_done++;
        }

        return *this;
    }

    Thread &handleHalt()
    {
        _curr_command.is_done = true;
        _commands_done++;
        _is_done = true;

        return *this;
    }

private: //members
    std::vector<int> _regs;
    uint _commands_done;
    Command _curr_command;
    bool _is_done;
    bool _is_accessed_memory_first_time;
};

class Core
{
public:
    Core(const uint &num_threads) :
        _num_threads(num_threads),
        _threads(num_threads),
        _cycles_count(0),
        _num_threads_remain(num_threads),
        _curr_thread(0),
        _switch_overhead(0)
    {

    }
    virtual Core &run() = 0;

    double calcCPI()
    {
        uint sum = 0;
        for(auto &thread : _threads)
        {
            sum += thread.getCommandsDone();
        }

        if(sum == 0)
        {
            return 0.0;
        }

        return static_cast<double >(_cycles_count) / static_cast<double>(sum);
    }

    Core &copyRegsToContext(tcontext *context, const uint &thread_id)
    {
        _threads[thread_id].copyRegsToContext(&context[thread_id]);

        return *this;
    }

    bool isThreadExists(const uint &id)
    {
        return id < _num_threads;
    }

    virtual Core &contextSwitch() = 0;

    virtual ~Core() = default;

protected: //methods
    Core &tik()
    {
        for(auto &thread : _threads)
        {
            thread.tik();
        }

        _cycles_count++;

        return *this;
    }

protected: //memners
    uint _num_threads;
    std::vector<Thread> _threads;
    uint _cycles_count;
    uint _num_threads_remain;
    uint _curr_thread;
    uint _switch_overhead;
};

class BlockedCore : public Core
{

public:

    BlockedCore(const uint &num_threads) : Core(num_threads)
    {
        _switch_overhead = static_cast<uint>(Get_switch_cycles());
    }

    virtual Core &run() override
    {
        while(_num_threads_remain > 0)
        {
            _threads[_curr_thread].readNextCommand(_curr_thread);
            _threads[_curr_thread].handleCommand();
            if(_threads[_curr_thread].isDone())
            {
                if(_num_threads_remain > 1)
                {
                  contextSwitch();
                }
                else
                {
                    tik();
                }
                _num_threads_remain--;
            }
            else if(_threads[_curr_thread].isAccesssedMemoryFirstTime())
            {
                contextSwitch();
            }
            else
            {
                tik();
            }
        }

        return *this;
    }

    virtual Core &contextSwitch() override
    {
        if(_num_threads_remain > 1)
        {
            do
            {
                _curr_thread = (_curr_thread + 1) % _num_threads;
            } while (_threads[_curr_thread].isDone());
        }

        for(uint i = 0; i < _switch_overhead + 1; i++)
        {
            tik();
        }
//        _cycles_count += static_cast<uint>(Get_switch_cycles());
        return *this;
    }
};

class FinedCore : public Core
{

public:

    FinedCore(const uint &num_threads) : Core(num_threads) {}

    virtual Core &run() override
    {
        while(_num_threads_remain > 0)
        {
            _threads[_curr_thread].readNextCommand(_curr_thread);
            _threads[_curr_thread].handleCommand();
            if(_threads[_curr_thread].isDone())
            {
                contextSwitch();
                _num_threads_remain--;
            }
            else
            {
                contextSwitch();
            }
        }

        return *this;
    }

    virtual Core &contextSwitch() override
    {
        if(_num_threads_remain > 1)
        {
            do
            {
                _curr_thread = (_curr_thread + 1) % _num_threads;
            } while (_threads[_curr_thread].isDone());
        }

        tik();
        return *this;
    }
};

static std::shared_ptr<BlockedCore> blocked_core;
static std::shared_ptr<FinedCore> fined_core;

Status Core_Context_switch_NoOverhead(int newthread){
    return Success;
}


Status Core_blocked_Multithreading(){

    blocked_core = std::make_shared<BlockedCore>(Get_thread_number());
    blocked_core->run();

    return Success;
}


Status Core_fineGrained_Multithreading(){

    fined_core = std::make_shared<FinedCore>(Get_thread_number());
    fined_core->run();

    return Success;
}


double Core_finegrained_CPI(){

    return fined_core->calcCPI();
}
double Core_blocked_CPI(){

    return blocked_core->calcCPI();
}

Status Core_blocked_context(tcontext* bcontext,int threadid){

    uint thread_id = static_cast<uint>(threadid);

    if(bcontext == nullptr || !blocked_core->isThreadExists(thread_id))
    {
        return Failure;
    }

    blocked_core->copyRegsToContext(bcontext, thread_id);

    return Success;
}

Status Core_finegrained_context(tcontext* finegrained_context,int threadid){

    uint thread_id = static_cast<uint>(threadid);

    if(finegrained_context == nullptr || !fined_core->isThreadExists(thread_id))
    {
        return Failure;
    }

    fined_core->copyRegsToContext(finegrained_context, thread_id);

    return Success;
}
