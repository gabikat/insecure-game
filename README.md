## (Insecure) Guessing Game

#### Game: Guess a random number. Your score is determined by the number of times your were consecutively within 35 steps of the random numbers generated.

Vulnerabilities:
-  Store sensitive info in data section of executable.. in plain text 😱
- **Shell injection** and **privilege escalation** (due to poor mediation)
- **Integer overflow** to overrun the **whitelist** check. 
    An attempt to increase security was made in using a whitelist instead of a blacklist for input validation, but the input from the user is gathered using deprecated (due to security vulnerabilities) functions.

These vulnerabilities allow an adversary to collect potentially sensitive data and alter the game's current and historic scores and leaderboards.