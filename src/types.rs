pub enum Sentence {
    True,
    False,
    Symbol(String),
    Not(Box<Sentence>),
    And(Vec<Sentence>),
    Or(Vec<Sentence>),
    Imp(Box<Sentence>, Box<Sentence>),
    Iff(Box<Sentence>, Box<Sentence>)
}

impl Sentence {
    pub fn symbol(s: String) -> Sentence {
        Sentence::Symbol(s)
    }

    pub fn not(s: Sentence) -> Sentence {
        Sentence::Not(Box::new(s))
    }

    pub fn and(s: Vec<Sentence>) -> Sentence {
        Sentence::And(s)
    }

    pub fn or(s: Vec<Sentence>) -> Sentence {
        Sentence::Or(s)
    }

    pub fn imp(sl: Sentence, sr: Sentence) -> Sentence {
        Sentence::Imp(Box::new(sl), Box::new(sr))
    }

    pub fn iff(sl: Sentence, sr: Sentence) -> Sentence {
        Sentence::Iff(Box::new(sl), Box::new(sr))
    }
}

