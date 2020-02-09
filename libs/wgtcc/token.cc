#include "token.h"

#include "mem_pool.h"
#include "parser.h"

#include <sstream>


namespace pp
{
    static MemPoolImp<Token> tokenPool;

    const std::unordered_map<std::string, int> Token::kwTypeMap_{
      { "auto", Token::WTOK_AUTO },
      { "break", Token::WTOK_BREAK },
      { "case", Token::WTOK_CASE },
      { "char", Token::WTOK_CHAR },
      { "const", Token::WTOK_CONST },
      { "continue", Token::WTOK_CONTINUE },
      { "default", Token::WTOK_DEFAULT },
      { "do", Token::WTOK_DO },
      { "double", Token::WTOK_DOUBLE },
      { "else", Token::WTOK_ELSE },
      { "enum", Token::WTOK_ENUM },
      { "extern", Token::WTOK_EXTERN },
      { "float", Token::WTOK_FLOAT },
      { "for", Token::WTOK_FOR },
      { "goto", Token::WTOK_GOTO },
      { "if", Token::WTOK_IF },
      { "inline", Token::WTOK_INLINE },
      { "int", Token::WTOK_INT },
      { "long", Token::WTOK_LONG },
      { "signed", Token::WTOK_SIGNED },
      { "unsigned", Token::WTOK_UNSIGNED },
      { "register", Token::WTOK_REGISTER },
      { "restrict", Token::WTOK_RESTRICT },
      { "return", Token::WTOK_RETURN },
      { "short", Token::WTOK_SHORT },
      { "sizeof", Token::WTOK_SIZEOF },
      { "static", Token::WTOK_STATIC },
      { "struct", Token::WTOK_STRUCT },
      { "switch", Token::WTOK_SWITCH },
      { "typedef", Token::WTOK_TYPEDEF },
      { "union", Token::WTOK_UNION },
      { "void", Token::WTOK_VOID },
      { "volatile", Token::WTOK_VOLATILE },
      { "while", Token::WTOK_WHILE },
      { "_Alignas", Token::WTOK_ALIGNAS },
      { "_Alignof", Token::WTOK_ALIGNOF },
      { "_Atomic", Token::WTOK_ATOMIC },
      { "__attribute__", Token::WTOK_ATTRIBUTE },
      { "_Bool", Token::WTOK_BOOL },
      { "_Complex", Token::WTOK_COMPLEX },
      { "_Generic", Token::WTOK_GENERIC },
      { "_Imaginary", Token::WTOK_IMAGINARY },
      { "_Noreturn", Token::WTOK_NORETURN },
      { "_Static_assert", Token::WTOK_STATIC_ASSERT },
      { "_Thread_local", Token::WTOK_THREAD },
    };

    const std::unordered_map<int, const char*> Token::tagLexemeMap_{
      { '(', "(" },
      { ')', ")" },
      { '[', "[" },
      { ']', "]" },
      { ':', ":" },
      { ',', "," },
      { ';', ";" },
      { '+', "+" },
      { '-', "-" },
      { '*', "*" },
      { '/', "/" },
      { '|', "|" },
      { '&', "&" },
      { '<', "<" },
      { '>', ">" },
      { '=', "=" },
      { '.', "." },
      { '%', "%" },
      { '{', "{" },
      { '}', "}" },
      { '^', "^" },
      { '~', "~" },
      { '!', "!" },
      { '?', "?" },
      { '#', "#" },

      { Token::WTOK_DSHARP, "##" },
      { Token::WTOK_PTR, "->" },
      { Token::WTOK_INC, "++" },
      { Token::WTOK_DEC, "--" },
      { Token::WTOK_LEFT, "<<" },
      { Token::WTOK_RIGHT, ">>" },
      { Token::WTOK_LE, "<=" },
      { Token::WTOK_GE, ">=" },
      { Token::WTOK_EQ, "==" },
      { Token::WTOK_NE, "!=" },
      { Token::WTOK_LOGICAL_AND, "&&" },
      { Token::WTOK_LOGICAL_OR, "||" },
      { Token::WTOK_MUL_ASSIGN, "*=" },
      { Token::WTOK_DIV_ASSIGN, "/=" },
      { Token::WTOK_MOD_ASSIGN, "%=" },
      { Token::WTOK_ADD_ASSIGN, "+=" },
      { Token::WTOK_SUB_ASSIGN, "-=" },
      { Token::WTOK_LEFT_ASSIGN, "<<=" },
      { Token::WTOK_RIGHT_ASSIGN, ">>=" },
      { Token::WTOK_AND_ASSIGN, "&=" },
      { Token::WTOK_XOR_ASSIGN, "^=" },
      { Token::WTOK_OR_ASSIGN, "|=" },
      { Token::WTOK_ELLIPSIS, "..." },

      { Token::WTOK_AUTO, "auto" },
      { Token::WTOK_BREAK, "break" },
      { Token::WTOK_CASE, "case" },
      { Token::WTOK_CHAR, "char" },
      { Token::WTOK_CONST, "const" },
      { Token::WTOK_CONTINUE, "continue" },
      { Token::WTOK_DEFAULT, "default" },
      { Token::WTOK_DO, "do" },
      { Token::WTOK_DOUBLE, "double" },
      { Token::WTOK_ELSE, "else" },
      { Token::WTOK_ENUM, "enum" },
      { Token::WTOK_EXTERN, "extern" },
      { Token::WTOK_FLOAT, "float" },
      { Token::WTOK_FOR, "for" },
      { Token::WTOK_GOTO, "goto" },
      { Token::WTOK_IF, "if" },
      { Token::WTOK_INLINE, "inline" },
      { Token::WTOK_INT, "int" },
      { Token::WTOK_LONG, "long" },
      { Token::WTOK_SIGNED, "signed" },
      { Token::WTOK_UNSIGNED, "unsigned" },
      { Token::WTOK_REGISTER, "register" },
      { Token::WTOK_RESTRICT, "restrict" },
      { Token::WTOK_RETURN, "return" },
      { Token::WTOK_SHORT, "short" },
      { Token::WTOK_SIZEOF, "sizeof" },
      { Token::WTOK_STATIC, "static" },
      { Token::WTOK_STRUCT, "struct" },
      { Token::WTOK_SWITCH, "switch" },
      { Token::WTOK_TYPEDEF, "typedef" },
      { Token::WTOK_UNION, "union" },
      { Token::WTOK_VOID, "void" },
      { Token::WTOK_VOLATILE, "volatile" },
      { Token::WTOK_WHILE, "while" },
      { Token::WTOK_ALIGNAS, "_Alignas" },
      { Token::WTOK_ALIGNOF, "_Alignof" },
      { Token::WTOK_ATOMIC, "_Atomic" },
      { Token::WTOK_ATTRIBUTE, "__attribute__" },
      { Token::WTOK_BOOL, "_Bool" },
      { Token::WTOK_COMPLEX, "_Complex" },
      { Token::WTOK_GENERIC, "_Generic" },
      { Token::WTOK_IMAGINARY, "_Imaginary" },
      { Token::WTOK_NORETURN, "_Noreturn" },
      { Token::WTOK_STATIC_ASSERT, "_Static_assert" },
      { Token::WTOK_THREAD, "_Thread_local" },

      { Token::WTOK_END, "(eof)" },
      { Token::WTOK_IDENTIFIER, "(identifier)" },
      { Token::WTOK_CONSTANT, "(constant)" },
      { Token::WTOK_LITERAL, "(string literal)" },
    };


    Token* Token::New(int tag) {
        return new (tokenPool.Alloc()) Token(tag);
    }


    Token* Token::New(const Token& other) {
        return new (tokenPool.Alloc()) Token(other);
    }


    Token* Token::New(int tag,
        const SourceLocation& loc,
        const std::string& str,
        bool ws) {
        return new (tokenPool.Alloc()) Token(tag, loc, str, ws);
    }


    TokenSequence TokenSequence::GetLine() {
        auto begin = begin_;
        while (begin_ != end_ && (*begin_)->tag_ != Token::WTOK_NEW_LINE)
            ++begin_;
        auto end = begin_;
        return { tokList_, begin, end };
    }


    /*
     * If this seq starts from the begin of a line.
     * Called only after we have saw '#' in the token sequence.
     */
    bool TokenSequence::IsBeginOfLine() const {
        if (begin_ == tokList_->begin())
            return true;

        auto pre = begin_;
        --pre;

        // We do not insert a newline at the end of a source file.
        // Thus if two token have different filename, the second is
        // the begin of a line.
        return ((*pre)->tag_ == Token::WTOK_NEW_LINE ||
            (*pre)->loc_.filename_ != (*begin_)->loc_.filename_);
    }

    const Token* TokenSequence::Peek() const {
        static auto eof = Token::New(Token::WTOK_END);
        if (begin_ != end_ && (*begin_)->tag_ == Token::WTOK_NEW_LINE) {
            ++begin_;
            return Peek();
        }
        else if (begin_ == end_) {
            if (end_ != tokList_->begin())
                *eof = *Back();
            eof->tag_ = Token::WTOK_END;
            return eof;
        }
        else if (parser_ && (*begin_)->tag_ == Token::WTOK_IDENTIFIER &&
            (*begin_)->str_ == "__func__") {
            auto filename = Token::New(*(*begin_));
            filename->tag_ = Token::WTOK_LITERAL;
            filename->str_ = "\"" + parser_->CurFunc()->Name() + "\"";
            *begin_ = filename;
        }
        return *begin_;
    }


    const Token* TokenSequence::Expect(int expect) {
        auto tok = Peek();
        if (!Try(expect)) {
            Error(tok, "'%s' expected, but got '%s'",
                Token::Lexeme(expect), tok->str_.c_str());
        }
        return tok;
    }

    void TokenSequence::Print(std::stringstream& ss) const
    {
        unsigned lastLine = 0;
        auto ts = *this;
        while (!ts.Empty()) {
            auto tok = ts.Next();
            if (lastLine != tok->loc_.line_) {
                ss << "\n";
                if (tok->loc_.line_ - lastLine > 1 || lastLine == 0)
                    ss << "#line " << tok->loc_.line_ << "\n";
                for (unsigned i = 0; i < tok->loc_.column_; ++i)
                    ss << ' ';
            }
            else if (tok->ws_) {
                ss << ' ';
            }
            ss << tok->str_;
            lastLine = tok->loc_.line_;
        }
        ss << "\n";
    }

    void TokenSequence::Print(FILE* fp) const {
        std::stringstream ss;
        Print(ss);
        fprintf(fp, "%s", ss.str().c_str());
    }
}
