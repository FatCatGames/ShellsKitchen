#include <chrono>
#include "PerformanceTimer.h"
//enum PerformanceProfilerFlags
//{
//	None = 0,
//	ShowParentProcetage = 1 << 1,
//	ShowWholeProcetage = 1 << 2
//};

class PerformanceManager
{
public:
	static void BeginTimer(const std::string& aName, const std::string& aParent/*, PerformanceProfilerFlags aFlags*/);
	static void StopTimer(const std::string& aName);
	static PerformanceTimer GetWholeFrame();
	static void ClearTimers();
private:

	static PerformanceTimer myWholeFrame;
	static std::vector<PerformanceTimer> myCurrentTimers;

	static std::vector<std::chrono::time_point<std::chrono::system_clock>> myStarts;
	static bool myStartedTimer;

	static void AddTimerToWholeFrame(PerformanceTimer aTimer);
	static PerformanceTimer* ParentExists(const std::string& aParentName, PerformanceTimer* aTimer);
	static void SortChildren(PerformanceTimer* aTimer);
};
