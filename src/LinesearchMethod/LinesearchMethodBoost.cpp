#include "LinesearchMethodBoost.h"

LinesearchMethodBoost::LinesearchMethodBoost()
{
	processInfo = ProcessInfo::getInstance();
	settings = SHOTSettings::Settings::getInstance();
}

LinesearchMethodBoost::~LinesearchMethodBoost()
{
}

class Test
{
	private:
		std::vector<double> firstPt;
		std::vector<double> secondPt;
		OptProblemOriginal *originalProblem;
		//std::vector<char> varTypes;

	public:
		Test(std::vector<double> ptA, std::vector<double> ptB, OptProblemOriginal *prob)
		{
			firstPt = ptA;
			secondPt = ptB;
			originalProblem = prob;

			//auto varTypes = originalProblem->getVariableTypes();
			/*
			 for (int i = 0; i < originalProblem->getNumberOfVariables();i++)
			 {
			 if (varTypes.at(i) == 'B' || varTypes.at(i) == 'I')
			 {
			 std::cout << "changing point from " << secondPt.at(i) " to " <<firstPt.at(i) << std::endl;
			 secondPt.at(i) = firstPt.at(i);
			 }
			 }*/
		}

		double operator()(const double x)
		{
			int length = firstPt.size();
			std::vector<double> ptNew(length);

			for (int i = 0; i < length; i++)
			{
				ptNew.at(i) = x * firstPt.at(i) + (1 - x) * secondPt.at(i);
			}

			auto validNewPt = originalProblem->getMostDeviatingConstraint(ptNew).value;

			return validNewPt;
		}
};

class TerminationCondition
{
	private:
		double tol;

	public:
		TerminationCondition(double tolerance)
		{
			tol = tolerance;
		}

		bool operator()(double min, double max)
		{
			return abs(min - max) <= tol;
		}
};

std::vector<double> LinesearchMethodBoost::findZero(std::vector<double> ptA, std::vector<double> ptB, int Nmax,
		double delta)
{
	if (ptA.size() != ptB.size())
	{
		processInfo->logger.message(1) << " Linesearch error: sizes of points vary" << std::to_string(ptB.size())
				<< " and " << std::to_string(ptA.size()) << CoinMessageEol;
	}

	Test t(ptA, ptB, processInfo->originalProblem);
	int length = ptA.size();
	std::vector<double> ptNew(length);
	std::vector<double> ptNew2(length);

	typedef std::pair<double, double> Result;
	boost::uintmax_t max_iter = Nmax;

	try
	{
		Result r1 = boost::math::tools::toms748_solve(t, 0.0, 1.0, TerminationCondition(delta), max_iter);

		//if (r1.first < 0.00001)
		//	r1.first = 0;

		for (int i = 0; i < length; i++)
		{
			ptNew.at(i) = r1.first * ptA.at(i) + (1 - r1.first) * ptB.at(i);
		}

		//if (r1.first > 1-0.5)
		//std::cout << "Line search value: " << r1.first << std::endl;

		if (max_iter > settings->getIntSetting("LinesearchMaxIter", "Linesearch")) processInfo->logger.message(1)
				<< "Warning maximal number of line search iterations reached: " << (int) max_iter << CoinMessageEol;

		auto validNewPt = processInfo->originalProblem->isConstraintsFulfilledInPoint(ptNew);

		if (!validNewPt) // Outside feasible region
		{
			//processInfo->addDualSolutionCandidate(ptNew, E_DualSolutionSource::Linesearch,
			//		processInfo->getCurrentIteration()->iterationNumber);

			//processInfo->addPrimalSolutionCandidate(ptNew2, E_PrimalSolutionSource::Linesearch,
			//		processInfo->getCurrentIteration()->iterationNumber);

			return (ptNew);
		}

		//auto validNewPt2 = processInfo->originalProblem->isConstraintsFulfilledInPoint(ptNew2);

		//processInfo->logger.message(3) << " Linesearch completed in " << n << " iterations." << CoinMessageEol;

		//processInfo->addDualSolutionCandidate(ptNew2, E_DualSolutionSource::Linesearch,
		//processInfo->getCurrentIteration()->iterationNumber);

		//processInfo->addPrimalSolutionCandidate(ptNew, E_PrimalSolutionSource::Linesearch,
		//		processInfo->getCurrentIteration()->iterationNumber);

		for (int i = 0; i < length; i++)
		{
			ptNew2.at(i) = r1.second * ptA.at(i) + (1 - r1.second) * ptB.at(i);
		}
		return ptNew2;
	}
	catch (std::exception e)
	{
		processInfo->logger.message(0) << "Boost error while doing linesearch: " << e.what() << CoinMessageEol;
		//processInfo->logger.message(0) << "Returning solution point instead. " << CoinMessageEol;
		//std::vector<double> ptEmpty(0);

		if (!processInfo->originalProblem->isConstraintsFulfilledInPoint(ptA)) //Returns the NLP point if not on the interior
		return ptA;

		return ptNew;
	}
	catch (...)
	{
		processInfo->logger.message(0) << "Boost error while doing linesearch." << CoinMessageEol;
		//processInfo->logger.message(0) << "Returning solution point instead. " << CoinMessageEol;

		if (!processInfo->originalProblem->isConstraintsFulfilledInPoint(ptA)) //Returns the NLP point if not on the interior
		return ptA;

		//std::vector<double> ptEmpty(0);
		return ptNew;
	}

}
