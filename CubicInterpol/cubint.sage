# Define a cubic polynomial p(x) such that:
#     p(a) = va, p'(a) = da
#     p(b) = vb, p'(b) = db
#
var("x a va da b vb db")
var("alpha beta")

p1 = va + (x - a)*(vb - va)/(b - a)
p2 = (x - a)*(x - b)
p3 = ((x - a)^2)*(x - b)

p = p1 + p2*alpha + p3*beta
dp = derivative(p, x)

res = solve([dp(x = a) == da, dp(x = b) == db], [alpha, beta])

al = res[0][0].rhs()
be = res[0][1].rhs()

h = p(alpha = al, beta = be)
print "Cubic pol. = ", h


