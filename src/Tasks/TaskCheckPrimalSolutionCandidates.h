/*
 * TaskCheckPrimalSolutionCandidates.h
 *
 *  Created on: Apr 7, 2015
 *      Author: alundell
 */

#pragma once

#include <TaskBase.h>

#include "../ProcessInfo.h"
#include "../PrimalSolutionStrategy/PrimalSolutionStrategyCheckPoint.h"

class TaskCheckPrimalSolutionCandidates: public TaskBase
{
	public:
		TaskCheckPrimalSolutionCandidates();
		virtual ~TaskCheckPrimalSolutionCandidates();
		virtual void run();
		virtual std::string getType();

	private:

		SHOTSettings::Settings *settings;
		ProcessInfo *processInfo;

		PrimalSolutionStrategyCheckPoint *primalStrategyCheckPoint;
};

