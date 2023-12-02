#pragma once

namespace Alpha::Core
{
	class LoggerSink
	{
	public:
		LoggerSink() = default;
		virtual ~LoggerSink() = default;
	
		LoggerSink(const LoggerSink&) = delete;
		LoggerSink& operator=(const LoggerSink&) = delete;
	
		virtual void Write(const char*, int) = 0;
		virtual void Flush() = 0;
	};

	class SinkMSVC : public LoggerSink
	{
		virtual void Write(const char* message, int count) override
		{
			count;
			OutputDebugStringA(message);
		}
	
		virtual void Flush() override
		{
		}
	};

	class SinkSTD : public LoggerSink
	{
		virtual void Write(const char* message, int count) override
		{
			count;
			std::cout << message;// << std::endl;
		}

		virtual void Flush() override
		{
		}
	};
}