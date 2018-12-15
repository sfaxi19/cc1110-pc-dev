#include "global.hpp"
#include "link_fsm.hpp"
#include "ISerialPort.hpp"

template<class SERIAL_PORT_TYPE>
class BoardClient
{
public:

	BoardClient(){}

	bool Connect();

private:
	SERIAL_PORT_TYPE m_serial_port;
	LinkFsm          m_link_fsm{m_serial_port};
};