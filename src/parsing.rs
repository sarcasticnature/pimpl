use logos::Logos;

#[derive(Logos, Debug, PartialEq)]
pub enum Token {
    #[error]
    #[regex(r"[ \t\f]+", logos::skip)]
    Error,

    #[regex(r"[a-zA-Z0-9_']+")]
    Symbol,

    #[token("\n")]
    NewLine,

    #[token("~")]
    Not,

    #[token("^")]
    And,

    #[token("v")]
    Or,

    #[token("->")]
    Imp,

    #[token("<=>")]
    Iff,

    #[token("(")]
    ParenL,

    #[token(")")]
    ParenR
}

