/* Subscriber class to process received messages in a callback based on a template type.
*
*  This class will handle the deserialization of the received payload and provide the
*  callback function with a reference to the object defined in the template.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__SUBSCRIBER_H
#define ARDUCOMM__SUBSCRIBER_H

#include "serialization.h"

namespace arducomm
{

/* Base Subscriber class with a default callback */
class Subscriber_
{
public:
    virtual void callback(uint8_t* buffer) = 0;
};

/* Specialized Subscriber class with a template for the mesage type */
template<class T>
class Subscriber: public Subscriber_
{
public:
    // Generic typedef for callback function
    typedef void(*f_callback_t)(const T&);

    Subscriber(f_callback_t callback) : callback_(callback) {}

    virtual void callback(uint8_t* buffer) override
    {
        this->callback_(parse<T>(buffer));
    }

private:
    f_callback_t callback_;
};

} // namespace arducomm

#endif
