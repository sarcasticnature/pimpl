module Main where

import qualified Pimpl (someFunc)

main :: IO ()
main = do
  putStrLn "Hello, Logic"
  Pimpl.someFunc
