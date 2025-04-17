#include "Common/SyscEvent.h"

typedef void* HANDLE;

class HSyncEventWin : public HSyncEvent
{
public:
	HSyncEventWin();
	~HSyncEventWin();

	virtual bool Create(bool bManualReset = false) override;

	virtual bool IsManualReset() const override;

	virtual void Trigger() override;

	virtual void Reset() override;

	virtual bool WaitFor(UInt32 WaitTime) override;

private:

	HANDLE Event;

	bool ManualReset;
};
