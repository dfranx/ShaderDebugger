#ifndef _WGTCC_TOKEN_H_
#define _WGTCC_TOKEN_H_

#include "error.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <unordered_map>


namespace pp
{
    class Generator;
    class Parser;
    class Scanner;
    class Token;
    class TokenSequence;

    using HideSet = std::set<std::string>;
    using TokenList = std::list<const Token*>;


    struct SourceLocation {
        const std::string* filename_;
        const char* lineBegin_;
        unsigned line_;
        unsigned column_;

        const char* Begin() const {
            return lineBegin_ + column_ - 1;
        }
    };


    class Token {
        friend class Scanner;
    public:
        enum {
            // Punctuators
            WTOK_LPAR = '(',
            WTOK_RPAR = ')',
            WTOK_LSQB = '[',
            WTOK_RSQB = ']',
            WTOK_COLON = ':',
            WTOK_COMMA = ',',
            WTOK_SEMI = ';',
            WTOK_ADD = '+',
            WTOK_SUB = '-',
            WTOK_MUL = '*',
            WTOK_DIV = '/',
            WTOK_OR = '|',
            WTOK_AND = '&',
            WTOK_XOR = '^',
            WTOK_LESS = '<',
            WTOK_GREATER = '>',
            WTOK_EQUAL = '=',
            WTOK_DOT = '.',
            WTOK_MOD = '%',
            WTOK_LBRACE = '{',
            WTOK_RBRACE = '}',
            WTOK_TILDE = '~',
            WTOK_NOT = '!',
            WTOK_COND = '?',
            WTOK_SHARP = '#',
            WTOK_NEW_LINE = '\n',

            WTOK_DSHARP = 128, // '##'
            WTOK_PTR,
            WTOK_INC,
            WTOK_DEC,
            WTOK_LEFT,
            WTOK_RIGHT,
            WTOK_LE,
            WTOK_GE,
            WTOK_EQ,
            WTOK_NE,
            WTOK_LOGICAL_AND,
            WTOK_LOGICAL_OR,

            WTOK_MUL_ASSIGN,
            WTOK_DIV_ASSIGN,
            WTOK_MOD_ASSIGN,
            WTOK_ADD_ASSIGN,
            WTOK_SUB_ASSIGN,
            WTOK_LEFT_ASSIGN,
            WTOK_RIGHT_ASSIGN,
            WTOK_AND_ASSIGN,
            WTOK_XOR_ASSIGN,
            WTOK_OR_ASSIGN,

            WTOK_ELLIPSIS,
            // Punctuators end

            // KEYWORD BEGIN
            // TYPE QUALIFIER BEGIN
            WTOK_CONST,
            WTOK_RESTRICT,
            WTOK_VOLATILE,
            WTOK_ATOMIC,
            // TYPE QUALIFIER END

            // TYPE SPECIFIER BEGIN
            WTOK_VOID,
            WTOK_CHAR,
            WTOK_SHORT,
            WTOK_INT,
            WTOK_LONG,
            WTOK_FLOAT,
            WTOK_DOUBLE,
            WTOK_SIGNED,
            WTOK_UNSIGNED,
            WTOK_BOOL,		// _Bool
            WTOK_COMPLEX,	// _Complex
            WTOK_STRUCT,
            WTOK_UNION,
            WTOK_ENUM,
            // TYPE SPECIFIER END

            WTOK_ATTRIBUTE, // GNU extension __attribute__
            // FUNCTION SPECIFIER BEGIN
            WTOK_INLINE,
            WTOK_NORETURN,	// _Noreturn
            // FUNCTION SPECIFIER END

            WTOK_ALIGNAS, // _Alignas
            // For syntactic convenience
            WTOK_STATIC_ASSERT, // _Static_assert
            // STORAGE CLASS SPECIFIER BEGIN
            WTOK_TYPEDEF,
            WTOK_EXTERN,
            WTOK_STATIC,
            WTOK_THREAD,	// _Thread_local
            WTOK_AUTO,
            WTOK_REGISTER,
            // STORAGE CLASS SPECIFIER END
            WTOK_BREAK,
            WTOK_CASE,
            WTOK_CONTINUE,
            WTOK_DEFAULT,
            WTOK_DO,
            WTOK_ELSE,
            WTOK_FOR,
            WTOK_GOTO,
            WTOK_IF,
            WTOK_RETURN,
            WTOK_SIZEOF,
            WTOK_SWITCH,
            WTOK_WHILE,
            WTOK_ALIGNOF, // _Alignof
            WTOK_GENERIC, // _Generic
            WTOK_IMAGINARY, // _Imaginary
            // KEYWORD END

            WTOK_IDENTIFIER,
            WTOK_CONSTANT,
            WTOK_I_CONSTANT,
            WTOK_C_CONSTANT,
            WTOK_F_CONSTANT,
            WTOK_LITERAL,

            // For the parser, a identifier is a typedef name or user defined type
            WTOK_POSTFIX_INC,
            WTOK_POSTFIX_DEC,
            WTOK_PREFIX_INC,
            WTOK_PREFIX_DEC,
            WTOK_ADDR,  // '&'
            WTOK_DEREF, // '*'
            WTOK_PLUS,
            WTOK_MINUS,
            WTOK_CAST,

            // For preprocessor
            WTOK_PP_IF,
            WTOK_PP_IFDEF,
            WTOK_PP_IFNDEF,
            WTOK_PP_ELIF,
            WTOK_PP_ELSE,
            WTOK_PP_ENDIF,
            WTOK_PP_INCLUDE,
            WTOK_PP_DEFINE,
            WTOK_PP_UNDEF,
            WTOK_PP_LINE,
            WTOK_PP_ERROR,
            WTOK_PP_PRAGMA,
            WTOK_PP_VERSION,
            WTOK_PP_NONE,
            WTOK_PP_EMPTY,


            WTOK_IGNORE,
            WTOK_INVALID,
            WTOK_END,
            WTOK_NOTOK = -1,
        };

        static Token* New(int tag);
        static Token* New(const Token& other);
        static Token* New(int tag,
            const SourceLocation& loc,
            const std::string& str,
            bool ws = false);
        Token& operator=(const Token& other) {
            tag_ = other.tag_;
            ws_ = other.ws_;
            loc_ = other.loc_;
            str_ = other.str_;
            hs_ = other.hs_ ? new HideSet(*other.hs_) : nullptr;
            return *this;
        }
        virtual ~Token() {}

        // Token::WTOK_NOTOK represents not a kw.
        static int KeyWordTag(const std::string& key) {
            auto kwIter = kwTypeMap_.find(key);
            if (kwTypeMap_.end() == kwIter)
                return Token::WTOK_NOTOK;	// Not a key word type
            return kwIter->second;
        }
        static bool IsKeyWord(const std::string& name);
        static bool IsKeyWord(int tag) { return WTOK_CONST <= tag && tag < WTOK_IDENTIFIER; }
        bool IsKeyWord() const { return IsKeyWord(tag_); }
        bool IsPunctuator() const { return 0 <= tag_ && tag_ <= WTOK_ELLIPSIS; }
        bool IsLiteral() const { return tag_ == WTOK_LITERAL; }
        bool IsConstant() const { return WTOK_CONSTANT <= tag_ && tag_ <= WTOK_F_CONSTANT; }
        bool IsIdentifier() const { return WTOK_IDENTIFIER == tag_; }
        bool IsEOF() const { return tag_ == Token::WTOK_END; }
        bool IsTypeSpecQual() const { return WTOK_CONST <= tag_ && tag_ <= WTOK_ENUM; }
        bool IsDecl() const { return WTOK_CONST <= tag_ && tag_ <= WTOK_REGISTER; }
        static const char* Lexeme(int tag) {
            auto iter = tagLexemeMap_.find(tag);
            if (iter == tagLexemeMap_.end())
                return nullptr;

            return iter->second;
        }

        int tag_;

        // 'ws_' standards for weither there is preceding white space
        // This is to simplify the '#' operator(stringize) in macro expansion
        bool ws_{ false };
        SourceLocation loc_;

        std::string str_;
        HideSet* hs_{ nullptr };

    private:
        explicit Token(int tag) : tag_(tag) {}
        Token(int tag, const SourceLocation& loc,
            const std::string& str, bool ws = false)
            : tag_(tag), ws_(ws), loc_(loc), str_(str) {}

        Token(const Token& other) {
            *this = other;
        }

        static const std::unordered_map<std::string, int> kwTypeMap_;
        static const std::unordered_map<int, const char*> tagLexemeMap_;
    };


    class TokenSequence {
        friend class Preprocessor;

    public:
        TokenSequence() : tokList_(new TokenList()),
            begin_(tokList_->begin()), end_(tokList_->end()) {}
        explicit TokenSequence(Token* tok) {
            TokenSequence();
            InsertBack(tok);
        }
        explicit TokenSequence(TokenList* tokList)
            : tokList_(tokList),
            begin_(tokList->begin()),
            end_(tokList->end()) {}
        TokenSequence(TokenList* tokList,
            TokenList::iterator begin,
            TokenList::iterator end)
            : tokList_(tokList), begin_(begin), end_(end) {}
        ~TokenSequence() {}
        TokenSequence(const TokenSequence& other) { *this = other; }
        const TokenSequence& operator=(const TokenSequence& other) {
            tokList_ = other.tokList_;
            begin_ = other.begin_;
            end_ = other.end_;
            return *this;
        }
        void Copy(const TokenSequence& other) {
            tokList_ = new TokenList(other.begin_, other.end_);
            begin_ = tokList_->begin();
            end_ = tokList_->end();
            for (auto iter = begin_; iter != end_; ++iter)
                *iter = Token::New(**iter);
        }
        void UpdateHeadLocation(const SourceLocation& loc) {
            assert(!Empty());
            auto tok = const_cast<Token*>(Peek());
            tok->loc_ = loc;
        }
        void FinalizeSubst(bool leadingWS, const HideSet& hs) {
            auto ts = *this;
            while (!ts.Empty()) {
                auto tok = const_cast<Token*>(ts.Next());
                if (!tok->hs_)
                    tok->hs_ = new HideSet(hs);
                else
                    tok->hs_->insert(hs.begin(), hs.end());
            }
            // Even if the token sequence is empty
            const_cast<Token*>(Peek())->ws_ = leadingWS;
        }

        const Token* Expect(int expect);
        bool Try(int tag) {
            if (Peek()->tag_ == tag) {
                Next();
                return true;
            }
            return false;
        }
        bool Test(int tag) { return Peek()->tag_ == tag; }
        const Token* Next() {
            auto ret = Peek();
            if (!ret->IsEOF()) {
                ++begin_;
                Peek(); // May skip newline token, but why ?
            }
            else {
                ++exceed_end;
            }
            return ret;
        }
        void PutBack() {
            assert(begin_ != tokList_->begin());
            if (exceed_end > 0) {
                --exceed_end;
            }
            else {
                --begin_;
                if ((*begin_)->tag_ == Token::WTOK_NEW_LINE)
                    PutBack();
            }
        }
        const Token* Peek() const;
        const Token* Peek2() {
            if (Empty())
                return Peek(); // Return the Token::WTOK_END
            Next();
            auto ret = Peek();
            PutBack();
            return ret;
        }
        const Token* Back() const {
            auto back = end_;
            return *--back;
        }
        void PopBack() {
            assert(!Empty());
            assert(end_ == tokList_->end());
            auto size_eq1 = tokList_->back() == *begin_;
            tokList_->pop_back();
            end_ = tokList_->end();
            if (size_eq1)
                begin_ = end_;
        }
        TokenList::iterator Mark() { return begin_; }
        void ResetTo(TokenList::iterator mark) { begin_ = mark; }
        bool Empty() const { return Peek()->tag_ == Token::WTOK_END; }
        void InsertBack(TokenSequence& ts) {
            auto pos = tokList_->insert(end_, ts.begin_, ts.end_);
            if (begin_ == end_) {
                begin_ = pos;
            }
        }
        void InsertBack(const Token* tok) {
            auto pos = tokList_->insert(end_, tok);
            if (begin_ == end_) {
                begin_ = pos;
            }
        }

        // If there is preceding newline
        void InsertFront(TokenSequence& ts) {
            auto pos = GetInsertFrontPos();
            begin_ = tokList_->insert(pos, ts.begin_, ts.end_);
        }
        void InsertFront(const Token* tok) {
            auto pos = GetInsertFrontPos();
            begin_ = tokList_->insert(pos, tok);
        }
        bool IsBeginOfLine() const;
        TokenSequence GetLine();
        void SetParser(Parser* parser) { parser_ = parser; }
        void Print(FILE* fp = stdout) const;
        void Print(std::stringstream& ss) const;

    private:
        // Find a insert position with no preceding newline
        TokenList::iterator GetInsertFrontPos() {
            auto pos = begin_;
            if (pos == tokList_->begin())
                return pos;
            --pos;
            while (pos != tokList_->begin() && (*pos)->tag_ == Token::WTOK_NEW_LINE)
                --pos;
            return ++pos;
        }

        TokenList* tokList_;
        mutable TokenList::iterator begin_;
        TokenList::iterator end_;
        Parser* parser_{ nullptr };
        int exceed_end{ 0 };
    };
}

#endif