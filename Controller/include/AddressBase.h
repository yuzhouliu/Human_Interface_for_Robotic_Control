//*****************************************************************************
//
// AddressBase.h
//
// Abstract base class for internet addresses.
//
// Created:
// December 27, 2015
//
// Modified:
// December 27, 2015
//
//*****************************************************************************
#ifndef _ADDRESSBASE_H_
#define _ADDRESSBASE_H_

template <class T>
class AddressBase
{
private:
    /* Fields */
    T _address;
    unsigned short _port;

public:
    /* Destructor */
    virtual ~AddressBase(){};

    /* Methods */
    virtual T getAddress();
    virtual void setAddress(T address);
    virtual unsigned short getPort();
    virtual void setPort(unsigned short port);
};

#endif /* _ADDRESSBASE_H_ */
