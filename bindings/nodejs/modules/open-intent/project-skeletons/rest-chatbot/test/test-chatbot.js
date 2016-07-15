var helpers = require('./helpers')('res');

describe('Test the chatbot with several scripts', function() {

    beforeEach(function(done) {
        helpers.beforeEach(done);
    });

    afterEach(function(done) {
        helpers.afterEach(done);
    });

    it('should handle a conversation in which the user order a burger', function(done) {
        var script = [
            "Hello\n",
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Hamburger\n",
            "Got it, you want hamburger, right?\n",
            "yes\n",
            "I'm ordering, it is gonna be 5$.\n"
        ];

        helpers.checkScript(script, done);
    });

    it('should handle a conversation in which the user order a salad', function(done) {
        var script = [
            "Hello\n",
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Salad\n",
            "Got it, you want salad, right?\n",
            "yes\n",
            "I'm ordering, it is gonna be 5$.\n"
        ];

        helpers.checkScript(script, done);
    });

    it('should handle a conversation in which the user order a pizza', function(done) {
        var script = [
            "Hello\n",
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Pizza\n",
            "Got it, you want pizza, right?\n",
            "yes\n",
            "I'm ordering, it is gonna be 5$.\n"
        ];

        helpers.checkScript(script, done);
    });

    it('should handle a conversation in which the chatbot does not understand the item', function(done) {
        var script = [
            "Hello\n",
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "chicken\n",
            "I did not understand. Pizza, hamburger or salad?\n"
        ];

        helpers.checkScript(script, done);
    });

    it('should handle a conversation in which the user made a mistake during ordering', function(done) {
        var script = [
            "Hello\n",
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Pizza\n",
            "Got it, you want pizza, right?\n",
            "No\n",
            "Tell me what you want. Pizza, hamburger or salad?\n",
            "Salad\n",
            "Got it, you want salad, right?\n",
            "yes\n",
            "I'm ordering, it is gonna be 5$.\n"

        ];

        helpers.checkScript(script, done);
    });
});
