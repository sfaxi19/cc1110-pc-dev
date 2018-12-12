#pragma once
#include <iostream>
#include "SerialPortConstants.h"

namespace LibSerial
{
class ISerialPort
{
public:

    virtual void Open(const std::string& fileName,
              const std::ios_base::openmode& openMode = std::ios_base::in | std::ios_base::out) {}
    virtual void Close(){}
	virtual void DrainWriteBuffer(){}
	virtual void FlushInputBuffer(){}
	virtual void FlushOutputBuffer(){}
	virtual void FlushIOBuffers(){}
	virtual bool IsDataAvailable(){}
	virtual bool IsOpen() const{ return true; }
	virtual void SetDefaultSerialPortParameters(){}
	virtual void SetBaudRate(const BaudRate& baudRate){}
	virtual BaudRate GetBaudRate() const{}
	virtual void SetCharacterSize(const CharacterSize& characterSize){}
	virtual CharacterSize GetCharacterSize() const{}
	virtual void SetFlowControl(const FlowControl& flowControlType){}
	virtual FlowControl GetFlowControl() const{}
	virtual void SetParity(const Parity& parityType){}
	virtual Parity GetParity() const {}
	virtual void SetStopBits(const StopBits& stopBits){}
	virtual StopBits GetStopBits() const{}
	virtual void SetVMin(short vmin){}
	virtual short GetVMin() const {}
	virtual void SetVTime(short vtime){}
	virtual short GetVTime() const{}
	virtual void SetDTR(bool dtrState = true){}
	virtual bool GetDTR() const{}
	virtual void SetRTS(bool rtsState = true){}
	virtual bool GetRTS() const{}
	virtual bool GetCTS(){}
	virtual bool GetDSR(){}
	virtual int GetFileDescriptor() const{}
	virtual int GetNumberOfBytesAvailable(){}
	virtual std::vector<std::string> GetAvailableSerialPorts() const{}
	virtual void Read(uint8_t* buffer,
              const size_t numberOfBytes,
              const size_t msTimeout){}
	
	virtual void Read(DataBuffer& dataBuffer,
              size_t      numberOfBytes = 0,
              size_t      msTimeout = 0,
              size_t      startPosition = 0) = 0;

    virtual void Read(std::string& dataString,
              size_t       numberOfBytes = 0,
              size_t       msTimeout = 0){}
    virtual void ReadByte(char&  charBuffer,
                  size_t msTimeout = 0){}
    virtual void ReadByte(unsigned char& charBuffer,
                  size_t         msTimeout = 0){}
    virtual void ReadLine(std::string& dataString,
                  char         lineTerminator = '\n',
                  size_t       msTimeout = 0){}
    virtual void Write(const DataBuffer& dataBuffer) = 0;
    virtual void Write(const std::string& dataString){}
    virtual void WriteByte(char charbuffer){}
    virtual void WriteByte(unsigned char charbuffer){}
    virtual void SetSerialPortBlockingStatus(bool blockingStatus){}
    virtual bool GetSerialPortBlockingStatus() const{}
    virtual void SetModemControlLine(int modemLine,
                             bool lineState){}
    virtual bool GetModemControlLine(int modemLine){}
};

}