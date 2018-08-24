#!/usr/bin/env stack
-- stack --resolver lts-12.6 --install-ghc runghc --package text

{-# LANGUAGE OverloadedStrings #-}

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
import qualified Data.Text as T
import qualified Data.Text.IO as T
import System.Environment

width :: Int
width = 70

main = do
  args <- getArgs
  forM_ args $ \arg -> do
    let title = T.pack arg
        semis = T.replicate width ";"
    T.putStrLn semis
    T.putStrLn $ ";;" <> T.center (width - 4) ' ' title <> ";;"
    T.putStrLn semis
    T.putStrLn ""
