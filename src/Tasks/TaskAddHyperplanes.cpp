/*
 * TaskAddHyperplanes.cpp
 *
 *  Created on: Mar 28, 2015
 *      Author: alundell
 */

#include <TaskAddHyperplanes.h>

TaskAddHyperplanes::TaskAddHyperplanes()
{
	processInfo = ProcessInfo::getInstance();
	settings = SHOTSettings::Settings::getInstance();
}

TaskAddHyperplanes::~TaskAddHyperplanes()
{
	// TODO Auto-generated destructor stub
}

void TaskAddHyperplanes::run()
{
	auto currIter = processInfo->getCurrentIteration(); // The unsolved new iteration

	if (!settings->getBoolSetting("DelayedConstraints", "MILP") || !currIter->isMILP()
			|| !currIter->MILPSolutionLimitUpdated)
	{
		for (int k = processInfo->hyperplaneWaitingList.size(); k > 0; k--)
		{
			auto tmpItem = processInfo->hyperplaneWaitingList.at(k - 1);

			auto tmpIdx = tmpItem.first;
			std::vector<double> tmpPts;

			tmpPts = tmpItem.second;

			createHyperplane(tmpIdx, tmpPts);
		}

		processInfo->hyperplaneWaitingList.clear();
	}
}

void TaskAddHyperplanes::createHyperplane(int constrIdx, std::vector<double> point)
{
	auto currIter = processInfo->getCurrentIteration(); // The unsolved new iteration
	auto originalProblem = processInfo->originalProblem;
	std::vector < IndexValuePair > elements;

	auto varNames = originalProblem->getVariableNames();

	processInfo->logger.message(3) << " HP point is: " << CoinMessageEol;

	for (int i = 0; i < point.size(); i++)
	{
		processInfo->logger.message(3) << "  " << varNames.at(i) << ": " << point[i] << CoinMessageEol;
	}

	double constant = originalProblem->calculateConstraintFunctionValue(constrIdx, point);

	if (constrIdx == -1)
	{
		processInfo->logger.message(3) << " HP point generated for auxiliary objective function constraint"
				<< CoinMessageEol;

		auto tmpArray = processInfo->originalProblem->getProblemInstance()->calculateObjectiveFunctionGradient(
				&point.at(0), -1, true);
		int number = processInfo->originalProblem->getNumberOfVariables();

		for (int i = 0; i < number - 1; i++)
		{
			if (tmpArray[i] != 0)
			{
				IndexValuePair pair;
				pair.idx = i;
				pair.value = tmpArray[i];

				elements.push_back(pair);
				constant += -tmpArray[i] * point.at(i);

				processInfo->logger.message(3) << " Gradient for variable" << varNames.at(i) << ": " << tmpArray[i]
						<< CoinMessageEol;
			}
		}

		processInfo->logger.message(3) << " Gradient for obj.var.: -1" << CoinMessageEol;

		IndexValuePair pair;
		pair.idx = processInfo->originalProblem->getNonlinearObjectiveVariableIdx();
		pair.value = -1.0;

		elements.push_back(pair);
		constant += -(-1) * point.at(pair.idx);
	}
	else
	{
		processInfo->logger.message(3) << " HP point generated for constraint index" << constrIdx << CoinMessageEol;

		auto nablag = originalProblem->calculateConstraintFunctionGradient(constrIdx, point);

		for (int i = 0; i < nablag->number; i++)
		{
			IndexValuePair pair;
			pair.idx = nablag->indexes[i];
			pair.value = nablag->values[i];

			elements.push_back(pair);
			constant += -nablag->values[i] * point.at(nablag->indexes[i]);

			processInfo->logger.message(3) << " Gradient for variable" << varNames.at(nablag->indexes[i]) << ": "
					<< nablag->values[i] << CoinMessageEol;
		}
	}

	for (auto E : elements)
	{
		processInfo->logger.message(3) << " HP coefficient for variable " << varNames.at(E.idx) << ": " << E.value
				<< CoinMessageEol;
	}

	processInfo->logger.message(3) << " HP constant " << constant << CoinMessageEol;

	processInfo->MILPSolver->addLinearConstraint(elements, elements.size(), constant);

	currIter->numHyperplanesAdded++;
	currIter->totNumHyperplanes++;

	if (settings->getBoolSetting("Debug", "SHOTSolver"))
	{
		stringstream ss;
		ss << settings->getStringSetting("DebugPath", "SHOTSolver");
		ss << "/lp";
		ss << currIter->iterationNumber - 1;
		ss << ".lp";
		processInfo->MILPSolver->writeProblemToFile(ss.str());
	}

	currIter->totNumHyperplanes = processInfo->getPreviousIteration()->totNumHyperplanes
			+ currIter->numHyperplanesAdded;
}
