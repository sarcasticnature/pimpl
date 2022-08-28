use logos::Logos;
use pimpl::parsing::Token;
use pimpl::types::Sentence;

fn main() {
    let string = String::from("(favorite | bar) -> (vest | testy)");
    let mut lex = Token::lexer(&string);
    let mut kb: Vec<Sentence> = Vec::new();

    println!("The string to parse is \"{}\"", string);
    while let Some(_) = lex.next() {
        println!("{}", lex.slice());
    }
}
