use std::rc::Rc;

pub enum Sentence {
    True,
    False,
    Symbol(String),
    Not(Rc<Sentence>),
    And(Vec<Rc<Sentence>>),
    Or(Vec<Rc<Sentence>>),
    Imp(Rc<Sentence>, Rc<Sentence>),
    Iff(Rc<Sentence>, Rc<Sentence>),
}

impl Sentence {
    pub fn symbol(s: String) -> Sentence {
        Sentence::Symbol(s)
    }

    pub fn not(s: Sentence) -> Sentence {
        Sentence::Not(Rc::new(s))
    }

    pub fn and(s: Vec<Rc<Sentence>>) -> Sentence {
        Sentence::And(s)
    }

    pub fn or(s: Vec<Rc<Sentence>>) -> Sentence {
        Sentence::Or(s)
    }

    pub fn imp(sl: Sentence, sr: Sentence) -> Sentence {
        Sentence::Imp(Rc::new(sl), Rc::new(sr))
    }

    pub fn iff(sl: Sentence, sr: Sentence) -> Sentence {
        Sentence::Iff(Rc::new(sl), Rc::new(sr))
    }
}
