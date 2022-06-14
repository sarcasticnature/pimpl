//use pimpl::types::Sentence;
use pimpl::parsing::Token;
use logos::Logos;

fn main() {
    println!("Hello, world!");

    let string = String::from("favorite ^ bar -> vest v testy");
    let mut lex = Token::lexer(&string);

    println!("The string to parse is \"{}\"", string);
    while let Some(_) = lex.next() {
        println!("{}", lex.slice());
    }
}
