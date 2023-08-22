#include <chrono>
#include <sstream>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>

using namespace std;
using namespace std::chrono;

class Show
{
public:
	int startTime;
	int endTime;
	string name;

	Show(int start, int end, string showname) : startTime(start), endTime(end), name(showname) {};
};

bool SortShows(const Show& showA, const Show& showB)
{
	//if shows have the same start time, then order based on end time
	return (showA.startTime == showB.startTime) ? (showA.endTime < showB.endTime) : (showA.startTime < showB.startTime);
}


/* Solution requires a sorted input.
 * While browsing through the vector of shows one can create a priority queue, which size corresponds to the number of the currently planned stages at that moment. This queue contains the end time of the last show planned for each stage, together with the stage number.
 * The ordering is such that the stage hosting the show with the earliest end time is on top of the queue (when same end time, the stage number is leading, the smaller is on top). 
 * The idea is that if input is sorted, one can go though the list of shows and check the "current" show end time and compare it with the top element of the queue.
 * If the "current" show end time is bigger than the smallest (minimum) end time of the last shows already planned for any of the already existing stages (aka the top of the queue), than the current show can fit the stage on top of the queue.
 * In this scenario, the queue is popped of the top value and it is added with the current show information (as it is the new last show of its stage).
 * Otherwise it cannot fit any stages and a new stage needs to be created. The queue is added with the information about the new stage (as it is the new last show of its stage).
 * Time complexity: ~O(nlog(n)) for sorting and ~O(nlog(n)) for the algorithm using the priority queue.
 */
class SolutionPriorityQueue
{
	class StageInfo
	{
	public:
		int lastShowEndtime; //end time of the last show which is planned for the specific stage
		int stageNumber;

		StageInfo(int showEndtime, int stageNum) : lastShowEndtime(showEndtime), stageNumber(stageNum) {};

		friend bool operator> (const StageInfo& lhs, const StageInfo& rhs)
		{
			//if last shows in the stages have the same end time, then prioritize show with lowest number (which was created first)
			return (lhs.lastShowEndtime == rhs.lastShowEndtime) ? (lhs.stageNumber > rhs.stageNumber) : (lhs.lastShowEndtime > rhs.lastShowEndtime);
		}
	};

public:
	void CreatePlanning(const vector<Show>& showsList, vector<vector<Show>>& planning)
	{
		int lastCreatedStageNumber = 0;

		//Defining a priority queue keeping track of the stage information (the last show end time which is planned for the specific stage)
		//The queue is ordered so that the stage hosting the show with earliest end-time is on top. 
		priority_queue<StageInfo, std::vector<StageInfo>, std::greater<StageInfo>> currentStagesInfoQueue;

		for (const Show& show : showsList)
		{
			bool canFitAlreadyExistingStage = false;

			if (!currentStagesInfoQueue.empty())
			{
				StageInfo stageWithEarliestEndTime = currentStagesInfoQueue.top();
				if (show.startTime > stageWithEarliestEndTime.lastShowEndtime)
				{
					//Show can fit already existing stage, so add it to the stage
					planning[stageWithEarliestEndTime.stageNumber].push_back(show);
					canFitAlreadyExistingStage = true;

					//update information in the queue as we added another show to the stage
					currentStagesInfoQueue.pop();
					StageInfo updatedStageInfo(show.endTime, stageWithEarliestEndTime.stageNumber);
					currentStagesInfoQueue.push(updatedStageInfo);
				}

			}
			if (!canFitAlreadyExistingStage)
			{
				vector<Show> newStage;
				newStage.push_back(show);
				planning.push_back(newStage);

				//update information in the queue as we added a show to a new stage
				StageInfo newStageInfo(show.endTime, lastCreatedStageNumber++);
				currentStagesInfoQueue.push(newStageInfo);
			}
		}
	}
};


/* Solution does not necessarily require a sorted input, however in order to present a sorted output sorting needs to happen at some point (before execution or after it on result itself).
* Approach is simpler, each input is compared with the shows already planned for each stage. As soon as the show can fit the stage (meaning it does not overlap with the other shows in the stage), it is planned for that stage and the check moves to the next show in the list.
* If it cannot fit any stage, then a new stage is created.
* Time complexity: ~O(nlog(n)) for sorting and ~O(n*n) for the algorithm using nested loops
*/
class SolutionWithNestedLoops
{
	bool AreShowsNotOverlapping(const Show& showA, const Show& showB)
	{
		return((showA.startTime < showB.startTime && showA.endTime< showB.startTime) ||
			(showB.startTime < showA.startTime && showB.endTime< showA.startTime));
	}

	bool AreShowsOverlapping(const Show& showA, const Show& showB)
	{
		return(!AreShowsNotOverlapping(showA, showB));
	}

	bool CanFitStage(Show& currentShow, vector<Show>& stage)
	{
		bool canFit = true;
		for (Show& show : stage)
		{
			if (AreShowsOverlapping(currentShow, show))
			{
				canFit = false;
				break;
			}
		}
		return canFit;
	}

public:
	void CreatePlanning(vector<Show>& showsList, vector<vector<Show>>& planning)
	{
		for (Show& show : showsList)
		{
			bool canFitAlreadyExistingStage = false;
			for (vector<Show>& stage : planning)
			{
				if (CanFitStage(show, stage))
				{
					canFitAlreadyExistingStage = true;
					stage.push_back(show);
					break;
				}
			}

			if (!canFitAlreadyExistingStage)
			{
				vector<Show> newStage;
				newStage.push_back(show);
				planning.push_back(newStage);
			}
		}
	}
};


/* Helper functions */
void PrintEvents(const vector<Show>& showsList)
{
	for (const Show& show : showsList)
	{
		cout << show.name << " :(" << show.startTime << " - " << show.endTime << ") --> ";
	}
	cout << " END" << endl;
}

void PrintPlanning(const vector<vector<Show>>& planning)
{
	int k = 0;
	for (const vector<Show>& stage : planning)
	{
		cout << "Stage " << ++k << " : ";
		PrintEvents(stage);
	}
}


bool ReadInputFromFile(string fileName, vector<Show>& shows)
{
	bool result = true;
	fstream file;
	file.open(fileName.c_str(), ios::in); 
	if(file.is_open())
	{
		string line;
		while (getline(file, line)) //read line
		{
			std::stringstream ss(line);
			string token;
			vector<string> showInfo;
			while (std::getline(ss, token, ' ')) //parse input per line
			{
				showInfo.push_back(token);
				token.clear();
			}
			if (showInfo.size() == 3) //if input is well formatted, 3 inputs are present per line (show name - show start time - show end time)
			{
				shows.push_back(Show(atoi(showInfo[1].c_str()), atoi(showInfo[2].c_str()), showInfo[0]));
			}
			else
			{
				result = false;
				break;
			}
		}
	}
	else result = false;
	file.close();

	return result;
}

int main()
{
	vector<Show> showList;
	if (ReadInputFromFile("input.txt", showList))
	{
		//Sort input
		sort(showList.begin(), showList.end(), SortShows);


		/* priority queue solution - preferred*/
		vector<vector<Show>> planningPriorityQueueSolution;
		auto start = high_resolution_clock::now();
		SolutionPriorityQueue().CreatePlanning(showList, planningPriorityQueueSolution);
		auto stop = high_resolution_clock::now();
		auto duration_sol_queue = duration_cast<microseconds>(stop - start);

		cout << "------------" << endl;
		cout << "SolutionWithPriorityQueue Duration : " << duration_sol_queue.count() << " microseconds" << endl;
		PrintPlanning(planningPriorityQueueSolution);


		/* nested loop solution - not preferred*/
		vector<vector<Show>> planningNestedSolution;
		start = high_resolution_clock::now();
		SolutionWithNestedLoops().CreatePlanning(showList, planningNestedSolution);
		stop = high_resolution_clock::now();
		auto duration_sol_nested = duration_cast<microseconds>(stop - start);

		cout << "------------" << endl;
		cout << "SolutionWithNestedLoops Duration : " << duration_sol_nested.count() << " microseconds" << endl;
		PrintPlanning(planningNestedSolution);
	}
	else cout << " invalid input" << endl;

	return 0;
}
