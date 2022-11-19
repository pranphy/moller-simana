#!//usr/bin/env bash
#SBATCH --ntasks=1
#SBATCH --job-name=pra-test-again
#SBATCH --output=test-log-here.log
#SBATCH --error=test-log-here.log
#SBATCH --mem-per-cpu=5000
#SBATCH --partition=production
#SBATCH --account=halla
#SBATCH --exclude=farm19104,farm19105,farm19106,farm19107,farm1996,farm19101
#
source /site/12gev_phys/softenv.sh 2.4

cd /w/halla-scshelf2102/moller12gev/pgautam/pf/sft/remoll/remoll

cmd="/w/halla-scshelf2102/moller12gev/pgautam/pf/sft/remoll/build/helicoil/remoll macros/begin-again-gary.mac"
exec ${cmd}
