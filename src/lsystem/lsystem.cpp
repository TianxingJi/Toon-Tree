#include "lsystem.h"

// Constructor
LSystem::LSystem(const std::string& axiom, const std::unordered_map<char, std::string>& rules, int iterations)
    : m_axiom(axiom), m_rules(rules), m_iterations(iterations), m_generatedString(axiom) {}

// Function to apply rules to a given string
std::string LSystem::applyRules(const std::string& input) {
    std::string output;

    // Iterate through each character in the input string
    for (char c : input) {
        // If the character has a replacement rule, use it; otherwise, keep the original
        if (m_rules.find(c) != m_rules.end()) {
            output += m_rules.at(c); // Apply the rule
        } else {
            output += c; // No rule, keep original character
        }
    }

    return output;
}

// Function to generate the final L-System string
std::string LSystem::generate() {
    m_generatedString = m_axiom; // Start with the axiom

    // Apply rules iteratively
    for (int i = 0; i < m_iterations; ++i) {
        m_generatedString = applyRules(m_generatedString);
    }

    return m_generatedString; // Return the final generated string
}
