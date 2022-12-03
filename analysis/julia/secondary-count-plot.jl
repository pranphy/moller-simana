using CSV
using DataFrames
using Plots
using StatsBase
using PraPhysics
using Printf

using PlotThemes
mythemes()
theme(:slide)

sim_id = "helicoil-20221121-113016"
img_id = sim_id * "-img"
fmt = "png"

primary_per_job = 10000
no_of_jobs = 1000

#prsr = "secondary"
prsr = "primary"

workdir="$(HOME)/simana"
imagedir = "$(workdir)/asset/image"
primary_count_file = "$(workdir)/sim/files/helicoil-20221121-113016-$(prsr)-count.csv"




df = CSV.read(primary_count_file,DataFrame,header=1)
#print(df)

totalsim = df[!, :TotalHit]
dethit = df[!,:DetHit]
tr1hit = df[!,:DetTrid1Hit]
sum(totalsim)
sum(dethit)
sum(tr1hit)
print(sum(dethit)/sum(totalsim))


h = fit(Histogram,dethit,nbins=30)

p = plot(size=(1100,680))
show_func!(h,p0=[350,2100,300.0])
plot!(xlabel="$(prsr) (hit.det==911)",ylabel="No of simulation files")
plot!(h)
#plot!(legend=:bottom)
savefig("$(imagedir)/$(img_id)-$(prsr)-det-hit-count-hist-not-normalized.$(fmt)")

p2 = plot(size(1000,400),label=nothing)
htr = fit(Histogram, tr1hit ,nbins=20)
plot!(htr)
show_func!(htr,p0=[125.0,11.0,3.0])
plot!(xlabel=" $(prsr) (hit.det==911 && hit.trid==1)",ylabel="No. of files")
savefig("$(imagedir)/$(img_id)-$(prsr)-trid-hit-count-hist.$(fmt)")

findethit = df[!,:FinDetHit]

h3 = fit(Histogram,findethit,nbins=10)
p = plot(size=(600,400))
plot!(h3,label="Total=$(sum(findethit))")
plot!(xlabel="$(prsr) (hit.det==28)",ylabel="No of sim. files")

savefig("$(imagedir)/helicoil-20221121-113016-$(prsr)-fin-det-hit.$(fmt)")
print(sum(totalsim))
