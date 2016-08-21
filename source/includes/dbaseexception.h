#ifndef DBASEEXCEPTION_H
#define DBASEEXCEPTION_H

#include <exception>
#include <QString>
#include <QByteArray>

class dBaseException : public std::exception {
public:
    explicit dBaseException(const QString& string) {_data = string.toLatin1(); }
    const char * what()     const noexcept         { return _data.constData(); }
private:
    QByteArray _data;
};

#endif // DBASEEXCEPTION_H
