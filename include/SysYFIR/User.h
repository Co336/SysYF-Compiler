#ifndef _SYSYF_USER_H_
#define _SYSYF_USER_H_

#include "Value.h"
#include <memory>
#include <vector>

namespace SysYF
{
namespace IR
{
class User : public Value
{
public:
    ~User() = default;

    // start from 0
    Ptr<Value> get_operand(unsigned i) const;

    auto &get_operands() { return operands_; };

    // start from 0
    void set_operand(unsigned i, Ptr<Value> v);
    void add_operand(Ptr<Value> v);

    unsigned get_num_operand() const;

    void remove_use_of_ops();
    void remove_operands(int index1,int index2);

protected:
    explicit User(Ptr<Type> ty, const std::string &name = "", unsigned num_ops = 0);

private:
    WeakPtrVec<Value> operands_;   // operands of this value
    unsigned num_ops_;
};

}
}
#endif // _SYSYF_USER_H_
