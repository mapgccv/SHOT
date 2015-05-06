/*
 * TaskSelectPrimalCandidatesFromLinesearch.h
 *
 *  Created on: Apr 7, 2015
 *      Author: alundell
 */

#pragma once

#include <TaskBase.h>
#include "../ProcessInfo.h"

#include "../LinesearchMethod/ILinesearchMethod.h"
#include "../LinesearchMethod/LinesearchMethodBisection.h"
#include "../LinesearchMethod/LinesearchMethodBoost.h"
#include "../MILPSolver/IMILPSolver.h"

class TaskSelectPrimalCandidatesFromLinesearch: public TaskBase
{
	public:
		TaskSelectPrimalCandidatesFromLinesearch();
		virtual ~TaskSelectPrimalCandidatesFromLinesearch();
		virtual void run();
		virtual std::string getType();
	private:
		SHOTSettings::Settings *settings;
		ProcessInfo *processInfo;
		ILinesearchMethod *linesearchMethod;
};

