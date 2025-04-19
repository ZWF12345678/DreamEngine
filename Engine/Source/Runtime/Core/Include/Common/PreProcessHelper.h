#pragma once

#define DE_PRIVATE_STRINGIZE(Token)                   #Token
#define DE_STRINGIZE(Token)                           DE_PRIVATE_STRINGIZE(Token)

#define DE_PRIVATE_JOIN(TokenA, TokenB)               TokenA##TokenB
#define DE_JOIN(TokenA, TokenB)                       DE_PRIVATE_JOIN(TokenA, TokenB)

#define DE_PRIVATE_JOIN_FIRST(Token, ...)             Token##__VA_ARGS__
#define DE_JOIN_FIRST(Token, ...)                     DE_PRIVATE_JOIN_FIRST(Token, __VA_ARGS__)

#define DE_PRIVATE_JOIN_FIRST(Token, ...)             Token##__VA_ARGS__
#define DE_JOIN_FIRST(Token, ...)                     DE_PRIVATE_JOIN_FIRST(Token, __VA_ARGS__)


#define DE_REMOVE_OPTIONAL_PARENS(...)                              DE_JOIN_FIRST(DE_PRIVATE_PREPROCESSOR_REMOVE_OPTIONAL_PARENS,DE_PRIVATE_PREPROCESSOR_REMOVE_OPTIONAL_PARENS __VA_ARGS__)
#define DE_PRIVATE_PREPROCESSOR_REMOVE_OPTIONAL_PARENS(...)         DE_PRIVATE_PREPROCESSOR_REMOVE_OPTIONAL_PARENS __VA_ARGS__
#define DE_PRIVATE_PREPROCESSOR_REMOVE_OPTIONAL_PARENSDE_PRIVATE_PREPROCESSOR_REMOVE_OPTIONAL_PARENS


#define PREPROCESSOR_TO_STRING(Token)                 DE_STRINGIZE(Token)
#define PREPROCESSOR_JOIN(TokenA, TokenB)             DE_JOIN(TokenA, TokenB)
#define PREPROCESSOR_JOIN_FIRST(Token, ...)           DE_JOIN_FIRST(Token, ##__VA_ARGS__)

#define COMPILED_PLATFORM_HEADER(Suffix)              PREPROCESSOR_TO_STRING(PREPROCESSOR_JOIN(PLATFORM_HEADER_NAME/PLATFORM_HEADER_NAME, Suffix))

#define PREPROCESSOR_REMOVE_OPTIONAL_PARENS(...)      DE_REMOVE_OPTIONAL_PARENS(__VA_ARGS__)