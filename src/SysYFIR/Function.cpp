#include "Module.h"
#include "Function.h"
#include "IRPrinter.h"
#include "internal_macros.h"

namespace SysYF
{
namespace IR
{
Function::Function(Ptr<FunctionType> ty, const std::string &name, Ptr<Module> parent)
    : Value(ty, name), parent_(parent), seq_cnt_(0)
{

}

void Function::init(Ptr<FunctionType> ty, const std::string &name, Ptr<Module> parent) {
    get_parent()->add_function(dynamic_pointer_cast<Function>(shared_from_this()));
    build_args();
}

Ptr<Function> Function::create(Ptr<FunctionType> ty, const std::string &name, Ptr<Module> parent)
{
    RET_AFTER_INIT(Function, ty, name, parent);
}

Ptr<FunctionType> Function::get_function_type() const
{
    return static_pointer_cast<FunctionType>(get_type());
}

Ptr<Type> Function::get_return_type() const
{
    return get_function_type()->get_return_type();
}

unsigned Function::get_num_of_args() const
{
    return get_function_type()->get_num_of_args();
}

unsigned Function::get_num_basic_blocks() const
{
    return basic_blocks_.size();
}

Ptr<Module> Function::get_parent() const
{
    return parent_.lock();
}

void Function::remove(Ptr<BasicBlock>  bb)
{ 
    basic_blocks_.remove(bb); 
    for (auto pre : bb->get_pre_basic_blocks()) 
    {
        pre.lock()->remove_succ_basic_block(bb);
    }
    for (auto succ : bb->get_succ_basic_blocks()) 
    {
        succ.lock()->remove_pre_basic_block(bb);
    }
}

void Function::build_args()
{
    auto func_ty = get_function_type();
    unsigned int num_args = get_num_of_args();
    for (unsigned int i = 0; i < num_args; i++) {
        arguments_.push_back(Argument::create(func_ty->get_param_type(i), "", dynamic_pointer_cast<Function>(shared_from_this()), i));
    }
}

void Function::add_basic_block(Ptr<BasicBlock> bb)
{
    basic_blocks_.push_back(bb);
}

void Function::set_instr_name()
{
    std::map<Ptr<Value> , int> seq;
    for (auto arg : this->get_args())
    {
        if ( seq.find(arg) == seq.end())
        {
            auto seq_num = seq.size() + seq_cnt_;
            if ( arg->set_name("arg"+std::to_string(seq_num) ))
            {
                seq.insert( {arg, seq_num} );
            }
        }
    }
    for (auto bb : basic_blocks_)
    {
        if ( seq.find(bb) == seq.end())
        {
            auto seq_num = seq.size() + seq_cnt_;
            if ( bb->set_name("label"+std::to_string(seq_num) ))
            {
                seq.insert( {bb, seq_num} );
            }
        }
        for (auto instr : bb->get_instructions())
        {
            if ( !instr->is_void() && seq.find(instr) == seq.end())
            {
                auto seq_num = seq.size() + seq_cnt_;
                if ( instr->set_name("op"+std::to_string(seq_num) ))
                {
                    seq.insert( {instr, seq_num} );
                }
            }
        }
    }
    seq_cnt_ += seq.size();
}

std::string Function::print()
{
    set_instr_name();
    std::string func_ir;
    if ( this->is_declaration() ) 
    {
        func_ir += "declare ";
    }    
    else
    {
        func_ir += "define ";
    }
    
    func_ir += this->get_return_type()->print();
    func_ir += " ";
    func_ir += print_as_op(shared_from_this(), false);
    func_ir += "(";

    //print arg
    if ( this->is_declaration() ) 
    {
        for (unsigned int i = 0; i < this->get_num_of_args(); i++)
        {
            if(i)
                func_ir += ", ";
            func_ir += static_pointer_cast<FunctionType>(this->get_type())->get_param_type(i)->print();
        }
    }
    else
    {
        for ( auto arg = this->arg_begin(); arg != arg_end() ; arg++ )
        {
            if( arg != this->arg_begin() )
            {
                func_ir += ", ";
            }
            func_ir += static_pointer_cast<Argument>(*arg)->print();
        }
    }
    func_ir += ")";

    //print bb
    if( this->is_declaration() ) {
        func_ir += "\n";
    }
    else
    {
        func_ir += " {";
        func_ir += "\n";
        for ( auto bb : this->get_basic_blocks() )
        {
            func_ir += bb->print();
        }
        func_ir += "}";
    }
    
    return func_ir;
}

Ptr<Argument> Argument::create(Ptr<Type> ty, const std::string &name, Ptr<Function> f,
                                unsigned arg_no)
{
    return Ptr<Argument>(new Argument(ty, name, f, arg_no));
}

std::string Argument::print()
{
    std::string arg_ir;
    arg_ir += this->get_type()->print();
    arg_ir += " %";
    arg_ir += this->get_name();
    return arg_ir;
}

}
}
