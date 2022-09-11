module Pimpl.Sentence
( Sentence
) where

-- A type to represent a sentence of propositional logic
data Sentence =
      False
    | True
    | Var String
    | Not Sentence
    | And Sentence Sentence
    | Or Sentence Sentence
    | Imp Sentence Sentence
    | Iff Sentence Sentence
    deriving (Eq)

