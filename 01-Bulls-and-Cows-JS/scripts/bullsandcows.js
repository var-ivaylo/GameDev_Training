function countBovine(num, guess) {
    var count = {bulls: 0, cows: 0},
        i;

    for (i = 0; i < num.length; i += 1) {
        if (num[i] === guess[i]) {
            count.bulls += 1;
        } else if (guess.search(num[i]) !== -1) {
            count.cows += 1;
        }
    }

    return count;
}

function hasDups(str) {
    var t = str.split('').sort(),
        i;

    for (i = 0; i < t.length; i += 1) {
        if (t[i] === t[i - 1]) {
            return true;
        }
    }

    return false;
}

function generateSecret(len, wordVersion) {
    var DIGITS = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9'],
        LETTERS = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'],
        codes = !wordVersion ? DIGITS : LETTERS;

    return codes.sort(function () {
        return Math.random() - 0.5;
    }).slice(0, len).join('');
}