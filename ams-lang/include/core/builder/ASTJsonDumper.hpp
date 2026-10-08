#pragma once
#include "AST.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>

namespace ams {

/**
 * @brief JSON AST Dumper for the AMS compiler.
 * 
 * Implements the ASTVisitor interface to serialize the AST into JSON format
 * compatible with the IDE's ASTPanel. Output conforms to:
 *   { type: string, name?: string, value?: string, children?: ASTNode[], line?: number }
 * 
 * Usage:
 *   ASTJsonDumper dumper(std::cout);
 *   dumper.dump(programNode);
 */
class ASTJsonDumper : public ASTVisitor {
public:
    explicit ASTJsonDumper(std::ostream& out) : out_(out) {}

    void dump(std::shared_ptr<ProgramNode> program) {
        if (program) {
            program->accept(this);
        } else {
            out_ << "{\"type\":\"Program\",\"children\":[]}";
        }
        out_ << std::endl;
    }

    // ── Visitor Overrides ────────────────────────────────────

    void visit(ProgramNode* node) override {
        out_ << "{\"type\":\"Program\"";
        emitLine(node);
        out_ << ",\"children\":[";
        for (size_t i = 0; i < node->programBlocks.size(); ++i) {
            if (i > 0) out_ << ",";
            node->programBlocks[i]->accept(this);
        }
        out_ << "]}";
    }

    void visit(GlobalSectionNode* node) override {
        out_ << "{\"type\":\"GlobalSection\"";
        emitLine(node);
        out_ << ",\"children\":[";
        for (size_t i = 0; i < node->statements.size(); ++i) {
            if (i > 0) out_ << ",";
            node->statements[i]->accept(this);
        }
        out_ << "]}";
    }

    void visit(ImportNode* node) override {
        out_ << "{\"type\":\"Import\"";
        emitLine(node);
        out_ << ",\"value\":" << jsonStr(node->moduleName) << "}";
    }

    void visit(MergeNode* node) override {
        out_ << "{\"type\":\"Merge\"";
        emitLine(node);
        out_ << ",\"value\":" << jsonStr(node->targetName) << "}";
    }

    void visit(SourceDefinitionNode* node) override {
        out_ << "{\"type\":\"SourceDefinition\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->sourceName);
        out_ << ",\"children\":[";
        bool first = true;
        if (node->schedule) {
            node->schedule->accept(this);
            first = false;
        }
        for (const auto& stmt : node->statements) {
            if (!first) out_ << ",";
            first = false;
            stmt->accept(this);
        }
        out_ << "]}";
    }

    void visit(EventDefinitionNode* node) override {
        out_ << "{\"type\":\"EventDefinition\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->eventName);
        out_ << ",\"value\":" << jsonStr(node->sourceName);
        out_ << ",\"children\":[";
        bool first = true;
        if (node->signalCondition) {
            out_ << "{\"type\":\"SignalCondition\",\"children\":[";
            node->signalCondition->accept(this);
            out_ << "]}";
            first = false;
        }
        for (const auto& stmt : node->statements) {
            if (!first) out_ << ",";
            first = false;
            stmt->accept(this);
        }
        out_ << "]}";
    }

    void visit(ObserverDefinitionNode* node) override {
        out_ << "{\"type\":\"ObserverDefinition\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->observerName);
        out_ << ",\"value\":" << jsonStr(node->observesEvent);
        out_ << ",\"children\":[";
        for (size_t i = 0; i < node->statements.size(); ++i) {
            if (i > 0) out_ << ",";
            node->statements[i]->accept(this);
        }
        out_ << "]}";
    }

    void visit(FunctionDefinitionNode* node) override {
        out_ << "{\"type\":\"FunctionDefinition\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->functionName);
        out_ << ",\"children\":[";
        bool first = true;
        // Emit parameters as children
        for (size_t i = 0; i < node->parameters.size(); ++i) {
            if (!first) out_ << ",";
            first = false;
            out_ << "{\"type\":\"Parameter\",\"name\":" << jsonStr(node->parameters[i]) << "}";
        }
        // Emit body statements
        for (const auto& stmt : node->statements) {
            if (!first) out_ << ",";
            first = false;
            stmt->accept(this);
        }
        out_ << "]}";
    }

    void visit(FunctionCallNode* node) override {
        out_ << "{\"type\":\"FunctionCall\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->functionName);
        if (!node->arguments.empty()) {
            out_ << ",\"children\":[";
            for (size_t i = 0; i < node->arguments.size(); ++i) {
                if (i > 0) out_ << ",";
                node->arguments[i]->accept(this);
            }
            out_ << "]";
        }
        out_ << "}";
    }

    void visit(LiteralNode* node) override {
        out_ << "{\"type\":\"Literal\"";
        emitLine(node);
        out_ << ",\"value\":" << jsonStr(node->value) << "}";
    }

    void visit(VariableNode* node) override {
        out_ << "{\"type\":\"Variable\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->name) << "}";
    }

    void visit(VariableDeclarationNode* node) override {
        out_ << "{\"type\":\"VariableDeclaration\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->varName);
        out_ << ",\"value\":" << jsonStr(node->dataType);
        if (node->isTrack) {
            out_ << ",\"children\":[{\"type\":\"Modifier\",\"value\":\"TRACK\"}";
            if (node->value) {
                out_ << ",";
                node->value->accept(this);
            }
            out_ << "]";
        } else if (node->isUnshare) {
            out_ << ",\"children\":[{\"type\":\"Modifier\",\"value\":\"UNSHARE\"}";
            if (node->value) {
                out_ << ",";
                node->value->accept(this);
            }
            out_ << "]";
        } else if (node->value) {
            out_ << ",\"children\":[";
            node->value->accept(this);
            out_ << "]";
        }
        out_ << "}";
    }

    void visit(AssignmentNode* node) override {
        out_ << "{\"type\":\"Assignment\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->varName);
        if (node->expression) {
            out_ << ",\"children\":[";
            node->expression->accept(this);
            out_ << "]";
        }
        out_ << "}";
    }

    void visit(UnaryOperatorNode* node) override {
        out_ << "{\"type\":\"UnaryOp\"";
        emitLine(node);
        out_ << ",\"value\":" << jsonStr(node->op);
        if (node->right) {
            out_ << ",\"children\":[";
            node->right->accept(this);
            out_ << "]";
        }
        out_ << "}";
    }

    void visit(BinaryOperatorNode* node) override {
        out_ << "{\"type\":\"BinaryOp\"";
        emitLine(node);
        out_ << ",\"value\":" << jsonStr(node->op);
        out_ << ",\"children\":[";
        if (node->left) node->left->accept(this);
        out_ << ",";
        if (node->right) node->right->accept(this);
        out_ << "]}";
    }

    void visit(IfStatementNode* node) override {
        out_ << "{\"type\":\"IfStatement\"";
        emitLine(node);
        out_ << ",\"children\":[";
        for (size_t i = 0; i < node->branches.size(); ++i) {
            if (i > 0) out_ << ",";
            auto& branch = node->branches[i];
            if (branch.condition) {
                out_ << "{\"type\":\"" << (i == 0 ? "If" : "ElseIf") << "Branch\",\"children\":[";
                // Condition
                out_ << "{\"type\":\"Condition\",\"children\":[";
                branch.condition->accept(this);
                out_ << "]},";
                // Body
                out_ << "{\"type\":\"Body\",\"children\":[";
                for (size_t j = 0; j < branch.body.size(); ++j) {
                    if (j > 0) out_ << ",";
                    branch.body[j]->accept(this);
                }
                out_ << "]}]}";
            } else {
                // Else branch (no condition)
                out_ << "{\"type\":\"ElseBranch\",\"children\":[";
                out_ << "{\"type\":\"Body\",\"children\":[";
                for (size_t j = 0; j < branch.body.size(); ++j) {
                    if (j > 0) out_ << ",";
                    branch.body[j]->accept(this);
                }
                out_ << "]}]}";
            }
        }
        out_ << "]}";
    }

    void visit(TimeStatementNode* node) override {
        out_ << "{\"type\":\"TimeStatement\"";
        emitLine(node);
        std::string typeStr;
        switch (node->type) {
            case TimeStatementNode::Type::EVERY:        typeStr = "EVERY"; break;
            case TimeStatementNode::Type::AT:           typeStr = "AT"; break;
            case TimeStatementNode::Type::CONTINUOUSLY: typeStr = "CONTINUOUSLY"; break;
            case TimeStatementNode::Type::DEFAULT:      typeStr = "DEFAULT"; break;
        }
        std::string valueStr = typeStr;
        if (node->value > 0) {
            valueStr += " " + std::to_string(node->value) + " " + node->unit;
        }
        out_ << ",\"value\":" << jsonStr(valueStr);
        out_ << "}";
    }

    void visit(DataAccessNode* node) override {
        out_ << "{\"type\":\"DataAccess\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->sourceName);
        out_ << ",\"value\":" << jsonStr(node->varName);
        out_ << "}";
    }

    void visit(SignalNode* node) override {
        out_ << "{\"type\":\"Signal\"";
        emitLine(node);
        if (node->condition) {
            out_ << ",\"children\":[";
            node->condition->accept(this);
            out_ << "]";
        }
        out_ << "}";
    }

    void visit(LogSourceOpenNode* node) override {
        out_ << "{\"type\":\"LogSourceOpen\"";
        emitLine(node);
        out_ << ",\"value\":" << jsonStr(node->filepath);
        out_ << ",\"children\":[{\"type\":\"AccessMode\",\"value\":" << jsonStr(node->accessMode) << "}]";
        out_ << "}";
    }

    void visit(MethodCallNode* node) override {
        out_ << "{\"type\":\"MethodCall\"";
        emitLine(node);
        out_ << ",\"name\":" << jsonStr(node->objectName + "." + node->methodName);
        if (!node->arguments.empty()) {
            out_ << ",\"children\":[";
            for (size_t i = 0; i < node->arguments.size(); ++i) {
                if (i > 0) out_ << ",";
                node->arguments[i]->accept(this);
            }
            out_ << "]";
        }
        out_ << "}";
    }

private:
    std::ostream& out_;

    // Emit line number if available
    void emitLine(ASTNode* node) {
        if (node->line > 0) {
            out_ << ",\"line\":" << node->line;
        }
    }

    // Escape a string for JSON output
    static std::string jsonStr(const std::string& s) {
        std::ostringstream oss;
        oss << "\"";
        for (char c : s) {
            switch (c) {
                case '"':  oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        oss << "\\u" << std::hex << std::setfill('0') << std::setw(4) << static_cast<int>(c);
                    } else {
                        oss << c;
                    }
            }
        }
        oss << "\"";
        return oss.str();
    }
};

} // namespace ams
