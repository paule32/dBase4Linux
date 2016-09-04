#ifndef DBASEVARIABLES_H
#define DBASEVARIABLES_H

#include "qmymainwindow.h"
#include "if_expr_op.h"

namespace dBaseParser {
bool textcursor_in_if_block = false;

enum dBaseTypes {
    unknown,
    b_value,        // bool
    p_value,        // parameter
    w_value,        // widget type
    m_value,
    c_value,
    i_value         // int type
};
dBaseTypes dBaseType;
class dBaseVariables
{
public:
    QString               data_name;
    QString               data_name_parent;
    dBaseTypes            data_type;
    dBaseTypes            data_type_extra;
    if_expr_op            data_value_if;
    int                   data_value_bool;
    int                   data_value_int;
    QMyMainWindow *       data_value_widget;
};
QVector <dBaseVariables*> dynamics;
QVector <dBaseVariables*> app_parameter;    // application parameters (forms)

QVector <QString> vec_push_1;  // lhs object
QVector <QString> vec_push_2;  // new object
QVector <QString> vec_push_3;  // rhs object
}  // namespace dBaseParser

#endif // DBASEVARIABLES_H

