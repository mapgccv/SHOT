/**
   The Supporting Hyperplane Optimization Toolkit (SHOT).

   @author Andreas Lundell, Åbo Akademi University

   @section LICENSE
   This software is licensed under the Eclipse Public License 2.0.
   Please see the README and LICENSE files for more information.
*/

#pragma once
#include "TaskBase.h"

namespace SHOT
{
struct Reformulation
{
    LinearConstraints linearConstraints;
    QuadraticConstraints quadraticConstraints;
    NonlinearConstraint nonlinearConstraint;

    AuxilliaryVariables reformulationVariables;
};

class TaskReformulateProblem : public TaskBase
{
public:
    TaskReformulateProblem(EnvironmentPtr envPtr);
    virtual ~TaskReformulateProblem();

    void run();
    virtual std::string getType();

private:
    bool useQuadraticConstraints = false;
    bool useQuadraticObjective = true;
    bool quadraticObjectiveRegardedAsNonlinear = false;
    bool PartitionQuadraticTermsInObjective = false;
    bool PartitionQuadraticTermsInConstraint = false;

    void reformulateObjectiveFunction();
    void reformulateConstraint(NumericConstraintPtr constraint);

    NonlinearExpressionPtr copyNonlinearExpression(NonlinearExpression* expression, const ProblemPtr destination);

    template <class T> void copyLinearTermsToConstraint(LinearTerms terms, T destination, bool reversedSigns = false);

    template <class T>
    void copyQuadraticTermsToConstraint(QuadraticTerms terms, T destination, bool reversedSigns = false);

    template <class T>
    void copyLinearTermsToObjectiveFunction(LinearTerms terms, T destination, bool reversedSigns = false);

    template <class T>
    void copyQuadraticTermsToObjectiveFunction(QuadraticTerms terms, T destination, bool reversedSigns = false);

    LinearTerms partitionNonlinearSum(const std::shared_ptr<ExpressionSum> source, bool reversedSigns);

    LinearTerms partitionNonlinearBinaryProduct(const std::shared_ptr<ExpressionSum> source, bool reversedSigns);

    std::tuple<LinearTerms, QuadraticTerms> reformulateAndPartitionQuadraticSum(
        const QuadraticTerms& quadraticTerms, bool reversedSigns, bool partitionNonBinaryTerms);
    std::tuple<LinearTerms, MonomialTerms> reformulateMonomialSum(
        const MonomialTerms& monomialTerms, bool reversedSigns);

    AuxilliaryVariablePtr getBilinearAuxilliaryVariable(VariablePtr firstVariable, VariablePtr secondVariable);

    void addBilinearMcCormickEnvelope(
        AuxilliaryVariablePtr auxVariable, VariablePtr firstVariable, VariablePtr secondVariable);

    int auxVariableCounter = 0;
    int auxConstraintCounter = 0;

    std::map<VariablePtr, Variables> integerAuxilliaryBinaryVariables;

    std::map<std::tuple<VariablePtr, VariablePtr>, AuxilliaryVariablePtr> bilinearAuxVariables;

    ProblemPtr reformulatedProblem;
};
} // namespace SHOT