#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <string>
#include <unordered_map>

class LSystem
{
public:
    // Constructor: Initializes the L-System with an axiom, rules, and iteration count
    LSystem(const std::string& axiom, const std::unordered_map<char, std::string>& rules, int iterations);

    // Generate the L-System string after applying the rules for the specified number of iterations
    std::string generate();

private:
    std::string m_axiom;                                   // The starting string (axiom)
    std::unordered_map<char, std::string> m_rules;         // Replacement rules for each character
    int m_iterations;                                      // Number of iterations to apply the rules
    std::string m_generatedString;                         // The final generated string

    // Function to apply rules to a given input string
    std::string applyRules(const std::string& input);      // Apply rules to transform the input string
};

#endif // LSYSTEM_H
