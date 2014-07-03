$(document).ready(function () {
    $('.play').on('click', function () {
        var secret = generateSecret($('.codes').val(), $('.wordVersion').is(':checked')),
            $history = $('.history'),
            $bulls = $('.bulls'),
            $cows = $('.cows'),
            won = false;

        $('#settings').hide();
        $('#input').show();

        console.log(secret);

        $('.guess').on('change', function () {
            var $this = $(this),
                guess = $this.val(),
                info;

            $this.val('');

            if (guess.length !== secret.length) {
                info = 'Your guess must be ' + secret.length + ' symbols long.';
            } else if (hasDups(guess)) {
                info = 'Your guess: #' + guess + ' has duplicated symbols';
            } else {
                var bovine = countBovine(secret, guess),
                    bulls = bovine.bulls,
                    cows = bovine.cows,
                    i;

                info = 'Your guess: #' + guess + ' has ' + bulls + ' bulls and ' + cows + ' cows';

                $bulls.children().hide();
                $cows.children().hide();
                for (i = 0; i < bulls; i += 1) {
                    $($bulls.children()[i]).show();
                }
                for (i = 0; i < cows; i += 1) {
                    $($cows.children()[i]).show();
                }

                if (bulls === secret.length) {
                    won = true;
                }
            }

            $history.append(info + '<br/>');
            $history.scrollTop($history[0].scrollHeight);

            // something fancy
            if (won) {
                $('.win').show();
                setTimeout(function () {
                    $('#settings').show();
                    $('#input').hide();
                    $bulls.children().show();
                    $cows.children().show();
                    $history.empty();
                    won = false;
                    $('.win').hide();
                    $('.guess').unbind('change');
                }, 4500);
            }
        });
    })
});