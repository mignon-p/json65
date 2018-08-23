#!/usr/bin/env stack
-- stack --resolver lts-12.6 --install-ghc runghc --package MonadRandom

{-
  JSON65 - A JSON parser for the 6502 microprocessor.

  https://github.com/ppelleti/json65

  Copyright © 2018 Patrick Pelletier

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-}

import Control.Monad
import Control.Monad.Random.Strict
import Data.Char
import Data.List
import Text.Printf

randomLiteral :: MonadRandom m => m String
randomLiteral = do
  x <- getRandomR (0, 2)
  return $ ["null", "false", "true"] !! x

randomInteger :: MonadRandom m => m String
randomInteger = do
  x <- getRandomR (-4294967296, 4294967296)
  return $ show (x :: Integer)

randomNumber :: MonadRandom m => m String
randomNumber = do
  x <- getRandom
  return $ show (x :: Float)

escapes :: [(Char, Char)]
escapes =
  [ ('\"', '\"')
  , ('\\', '\\')
  , ('\b', 'b')
  , ('\f', 'f')
  , ('\n', 'n')
  , ('\r', 'r')
  , ('\t', 't')
  ]

randomChar :: MonadRandom m => m String
randomChar = do
  x <- getRandomR (1, 126)
  let c = chr x
      esc = c `lookup` escapes
  case esc of
    (Just c') -> return $ ['\\', c']
    _ -> if x < 32
         then return $ printf "\\u%04x" x
         else return [c]

randomString :: MonadRandom m => m String
randomString = do
  len <- getRandomR (0, 6)
  chars <- replicateM len randomChar
  return $ "\"" ++ concat chars ++ "\""

randomArray :: MonadRandom m => Int -> m String
randomArray level = do
  len <- getRandomR (0, 5)
  values <- replicateM len (randomValue (level - 1))
  return $ "[" ++ intercalate "," values ++ "]"

mkPair :: String -> String -> String
mkPair k v = k ++ ":" ++ v

randomObject :: MonadRandom m => Int -> m String
randomObject level = do
  len <- getRandomR (0, 5)
  keys <- replicateM len randomString
  values <- replicateM len (randomValue (level - 1))
  let pairs = zipWith mkPair keys values
  return $ "{" ++ intercalate "," pairs ++ "}"

randomValue :: MonadRandom m => Int -> m String
randomValue level = do
  let highest = if level < 0 then 3 else 5
  x <- getRandomR (0, highest)
  case x of
    4 -> randomArray level
    5 -> randomObject level
    _ -> [randomLiteral, randomInteger, randomNumber, randomString] !! x

randomArrayOrObject :: MonadRandom m => Int -> m String
randomArrayOrObject level = do
  x <- getRandom
  if x then randomArray level else randomObject level

generateStuff :: Int -> IO ()
generateStuff level = do
  str <- randomArrayOrObject level
  when (length str < 2046) $ do
    putStrLn str
    generateStuff (level + 1)

main :: IO ()
main = generateStuff 0
