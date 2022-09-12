module Pimpl.Sentence
( Sentence
) where

import Data.Char
import Text.ParserCombinators.ReadP as ReadP


-- **Note:** this code is an almost exact copy of the code on this webpage:
-- http://cmsc-16100.cs.uchicago.edu/2017/Notes/propositional-logic-parsing.php
--
-- Credit is likely due to the listed instructors on the course's webpage,
-- namely Ravi Chugh and Stuart Kurtz (2017)
--
-- Minor modifications have been made out of personal preference, but the actual
-- algorithms are copied verbatim
--
-- I will likely replace the parsing code at some point, once I have a better
-- grasp of Haskell (if only for my own edification)

-- A type to represent a sentence of propositional logic
data Sentence =
      Var String
    | Boolean Bool
    | Not Sentence
    | And Sentence Sentence
    | Or Sentence Sentence
    | Imp Sentence Sentence
    | Iff Sentence Sentence
    deriving (Eq)

-- Strings which will be used to represent the logical operators in the text
-- a user inputs. This approach makes it easier to change the syntax later
falseT, trueT, notT, andT, orT, impT, iffT :: String
falseT = "False"
trueT = "True"
notT = "~"
andT = "&"
orT = "|"
impT = "->"
iffT = "<->"

means :: String -> a -> ReadP a
name `means` meaning = skipSpaces *> string name *> pure meaning

parseVar =
    skipSpaces *> (Var <$> ((:) <$> satisfy isLower <*> munch isAlphaNum))

prefix :: ReadP a -> ReadP (a -> a) -> ReadP a
prefix p op = result where
    result = p <++ (op <*> result)

parens :: ReadP a -> ReadP a
parens = between (skipSpaces *> char '(') (skipSpaces *> char ')')

instance Read Sentence where
    readsPrec _ = readP_to_S prec0 where
        prec0 = chainr1 prec1 (iffT `means` Iff)
        prec1 = chainr1 prec2 (impT `means` Imp)
        prec2 = chainl1 prec3 (orT `means` Or)
        prec3 = chainl1 prec4 (andT `means` And)
        prec4 = prefix prec5 (notT `means` Not)
        prec5 = parseVar <++ parens prec0 <++ parseBool
        parseVar = skipSpaces *> (Var <$> ((:) <$> satisfy isLower
                                               <*> munch isAlphaNum))
        parseBool = Boolean <$> (trueT `means` True <++ falseT `means` False)

instance Show Sentence where
    show = showS where
        showS (Var v) = v
        showS (Boolean True) = "True"
        showS (Boolean False) = "False"
        showS (Not x) = "~" ++ show x
        showS (And x y) = "(" ++ show x ++ " & " ++ show y ++ ")"
        showS (Or x y) = "(" ++ show x ++ " | " ++ show y ++ ")"
        showS (Imp x y) = "(" ++ show x ++ " -> " ++ show y ++ ")"
        showS (Iff x y) = "(" ++ show x ++ " <-> " ++ show y ++ ")"
