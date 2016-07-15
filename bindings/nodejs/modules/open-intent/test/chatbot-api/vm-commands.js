
var state = 4;

module.exports = {
    'command1': function(intentVariables, sessionId, next) {
        state += 1;
        next({state: state});
    },
    'command2': function(intentVariables, sessionId, next) {
        state += 2
        next({state: state});
    }
}